/*
 *  Copyright (C) 2006 HAS
 *
 *  Thanks to: FUJIMOTO Hisakuni, author of RubyAEOSA
 */

#include "osx_ruby.h"
#include <Carbon/Carbon.h>

VALUE rb_ll2big(LONG_LONG); // keeps gcc happy

// AE module and classes
static VALUE mAE;
static VALUE cAEDesc;
static VALUE cMacOSError;

// Note: AEDescs need extra wrapping to avoid nasty problems with Ruby's Data_Wrap_Struct.
struct rbAE_AEDescWrapper {
	AEDesc desc;
};

// (these two macros are basically cribbed from RubyAEOSA's aedesc.c)
#define AEDESC_DATA_PTR(o) ((struct rbAE_AEDescWrapper*)(DATA_PTR(o)))
#define AEDESC_OF(o) (AEDESC_DATA_PTR(o)->desc)

// Event handling
typedef long refcontype;

AEEventHandlerUPP upp_GenericEventHandler;
AECoercionHandlerUPP upp_GenericCoercionHandler;


/**********************************************************************/
// Raise MacOS error

/*
 * Note: MacOSError should only be raised by AE module; attempting to raise it from Ruby
 * just results in unexpected errors. (I've not quite figured out how to implement an
 * Exception class in C that constructs correctly in both C and Ruby. Not serious, since
 * nobody else needs to raise MacOSErrors - just a bit irritating.)
 */ 

static void
rbAE_raiseMacOSError(const char *description, OSErr number)
{
	VALUE errObj;
	
	errObj = rb_funcall(cMacOSError, rb_intern("new"), 0);
	rb_iv_set(errObj, "@number", INT2NUM(number)); // returns the OS error number
	rb_iv_set(errObj, "@description", rb_str_new2(description)); // troubleshooting info
	rb_exc_raise(errObj);
}


/**********************************************************************/
// MacOSError methods

static VALUE
rbAE_MacOSError_inspect(VALUE self)
{
	char s[32];
	
	sprintf(s, "#<AE::MacOSError %i>", NUM2INT(rb_iv_get(self, "@number")));
	return rb_str_new2(s);
}


/**********************************************************************/
// AEDesc support functions

static DescType
rbStringToDescType(VALUE obj)
{
	if (rb_obj_is_kind_of(obj, rb_cString) && RSTRING(obj)->len == 4) {
		return CFSwapInt32HostToBig(*(DescType *)(RSTRING(obj)->ptr));
	} else {
		rb_raise(rb_eArgError, "Not a four-char-code string.");
	}
}

static VALUE
rbDescTypeToString(DescType descType)
{
	char s[4];
	
	*(DescType*)s = CFSwapInt32HostToBig(descType);
	return rb_str_new(s, 4);
}

/*******/

static void
rbAE_freeAEDesc(struct rbAE_AEDescWrapper *p)
{
	AEDisposeDesc(&(p->desc));
	free(p);
}

static VALUE
rbAE_wrapAEDesc(const AEDesc *desc)
{
		struct rbAE_AEDescWrapper *wrapper;
		
		// Found out how to wrap AEDescs so Ruby wouldn't crash by reading RubyAEOSA's aedesc.c
		wrapper = malloc(sizeof(struct rbAE_AEDescWrapper));
		wrapper->desc = *desc;
		return Data_Wrap_Struct(cAEDesc, 0, rbAE_freeAEDesc, wrapper);
}

/*******/
// Note: clients should not attempt to use retain/use borrowed AE::AEDesc instances after handler callbacks return,
// as AEM will have disposed of the underlying AEDesc objects by then (TO DO: safety checking in AEDESC_OF?)

static void
rbAE_freeBorrowedAEDesc(struct rbAE_AEDescWrapper *p)
{
	free(p);
}

static VALUE
rbAE_wrapBorrowedAEDesc(const AEDesc *desc)
{
		struct rbAE_AEDescWrapper *wrapper;
		
		wrapper = malloc(sizeof(struct rbAE_AEDescWrapper));
		wrapper->desc = *desc;
		return Data_Wrap_Struct(cAEDesc, 0, rbAE_freeBorrowedAEDesc, wrapper);
}

/**********************************************************************/
// AEDesc constructors

static VALUE
rbAE_AEDesc_new(VALUE class, VALUE type, VALUE data)
{
	OSErr err = noErr;
	AEDesc desc;
	
	Check_Type(data, T_STRING);
	err = AECreateDesc(rbStringToDescType(type),
					   RSTRING(data)->ptr, RSTRING(data)->len,
					   &desc);
	if (err != noErr) rbAE_raiseMacOSError("Can't create AEDesc.", err);
	return rbAE_wrapAEDesc(&desc);
}


static VALUE
rbAE_AEDesc_newList(VALUE class, VALUE isRecord)
{
	OSErr err = noErr;
	AEDesc desc;
	
	err = AECreateList(NULL, 0, RTEST(isRecord), &desc);
	if (err != noErr) rbAE_raiseMacOSError("Can't create AEDescList.", err);
	return rbAE_wrapAEDesc(&desc);
}


static VALUE
rbAE_AEDesc_newAppleEvent(VALUE class, VALUE eventClass, VALUE eventID, 
		VALUE target, VALUE returnID, VALUE transactionID)
{
	OSErr err = noErr;
	AEDesc desc;
	
	err = AECreateAppleEvent(rbStringToDescType(eventClass), 
							 rbStringToDescType(eventID),
							 &(AEDESC_OF(target)),
							 NUM2INT(returnID), 
							 NUM2LONG(transactionID),
							 &desc);
	if (err != noErr) rbAE_raiseMacOSError("Can't create AppleEvent.", err);
	return rbAE_wrapAEDesc(&desc);
}


/**********************************************************************/
// AEDesc methods

static VALUE
rbAE_AEDesc_inspect(VALUE self)
{
	VALUE s, type;
	Size dataSize;
	
	s = rb_str_new2("#<AE::AEDesc type=%s size=%i>");
	type = rb_funcall(self, rb_intern("type"), 0);
	dataSize = AEGetDescDataSize(&(AEDESC_OF(self)));
	return rb_funcall(s, 
					  rb_intern("%"), 
					  1,
					  rb_ary_new3(2, 
								  rb_funcall(type, rb_intern("inspect"), 0),
								  INT2NUM(dataSize)
					  )
	);
}


/*******/

static VALUE
rbAE_AEDesc_type(VALUE self)
{	
	return rbDescTypeToString(AEDESC_OF(self).descriptorType);
}


static VALUE
rbAE_AEDesc_data(VALUE self)
{
	OSErr err = noErr;
	Size dataSize;
	void *data;
	VALUE result;
	
	dataSize = AEGetDescDataSize(&(AEDESC_OF(self)));
	data = malloc(dataSize);
	err = AEGetDescData(&(AEDESC_OF(self)), data, dataSize);
	if (err != noErr) rbAE_raiseMacOSError("Can't get AEDesc data.", err);
	result = rb_str_new(data, dataSize);
	free(data);
	return result;
}


/*******/

static VALUE
rbAE_AEDesc_isRecord(VALUE self)
{
	return AECheckIsRecord(&(AEDESC_OF(self))) ? Qtrue : Qfalse;
}

static VALUE
rbAE_AEDesc_coerce(VALUE self, VALUE type)
{
	OSErr err = noErr;
	AEDesc desc;
	
	err = AECoerceDesc(&(AEDESC_OF(self)), rbStringToDescType(type), &desc);
	if (err != noErr) rbAE_raiseMacOSError("Can't coerce AEDesc.", err);
	return rbAE_wrapAEDesc(&desc);
}

static VALUE
rbAE_AEDesc_length(VALUE self)
{
	OSErr err = noErr;
	long length;
	
	err = AECountItems(&(AEDESC_OF(self)), &length);
	if (err != noErr) rbAE_raiseMacOSError("Can't get length of AEDesc.", err);
	return INT2NUM(length);
}


static VALUE
rbAE_AEDesc_putItem(VALUE self, VALUE index, VALUE desc)
{
	OSErr err = noErr;
	
	if (rb_obj_is_instance_of(desc, cAEDesc) == Qfalse)
			rb_raise(rb_eTypeError, "Can't put non-AEDesc item into AEDesc.");
	err = AEPutDesc(&(AEDESC_OF(self)), NUM2LONG(index), &(AEDESC_OF(desc)));
	if (err != noErr) rbAE_raiseMacOSError("Can't put item into AEDesc.", err);
	return Qnil;
}


static VALUE
rbAE_AEDesc_putParam(VALUE self, VALUE key, VALUE desc)
{
	OSErr err = noErr;
	
	if (rb_obj_is_instance_of(desc, cAEDesc) == Qfalse)
			rb_raise(rb_eTypeError, "Can't put non-AEDesc parameter into AEDesc.");
	err = AEPutParamDesc(&(AEDESC_OF(self)), rbStringToDescType(key), &(AEDESC_OF(desc)));
	if (err != noErr) rbAE_raiseMacOSError("Can't put parameter into AEDesc.", err);
	return Qnil;
}


static VALUE
rbAE_AEDesc_putAttr(VALUE self, VALUE key, VALUE desc)
{
	OSErr err = noErr;
	
	if (rb_obj_is_instance_of(desc, cAEDesc) == Qfalse)
			rb_raise(rb_eTypeError, "Can't put non-AEDesc attribute into AEDesc.");
	err = AEPutAttributeDesc(&(AEDESC_OF(self)), rbStringToDescType(key), &(AEDESC_OF(desc)));
	if (err != noErr) rbAE_raiseMacOSError("Can't put attribute into AEDesc.", err);
	return Qnil;
}


static VALUE
rbAE_AEDesc_get(VALUE self, VALUE index, VALUE type)
{
	OSErr err = noErr;
	AEKeyword key;
	AEDesc desc;
	
	// TO DO: this gives bus error if typeAEList and index = 0 (should be OSErr -1701); why?
	err = AEGetNthDesc(&(AEDESC_OF(self)),
					   NUM2LONG(index),
					   rbStringToDescType(type),
					   &key,
					   &desc);
	if (err != noErr) rbAE_raiseMacOSError("Can't get item from AEDesc.", err);
	return rb_ary_new3(2,
					   rbDescTypeToString(key),
					   rbAE_wrapAEDesc(&desc));
}


static VALUE
rbAE_AEDesc_send(VALUE self, VALUE sendMode, VALUE timeout)
{
	OSErr err = noErr;
	AppleEvent reply;
	
	err = AESendMessage(&(AEDESC_OF(self)),
						&reply,
						(AESendMode)NUM2LONG(sendMode),
						NUM2LONG(timeout));
	if (err != noErr) rbAE_raiseMacOSError("Can't send Apple event.", err);
	return rbAE_wrapAEDesc(&reply);
}


/**********************************************************************/
// Find and launch applications

static VALUE
rbAE_findApplication(VALUE self, VALUE creator, VALUE bundleID, VALUE name)
{
	OSStatus err = 0;
	
	OSType inCreator;
	CFStringRef inName;
	CFStringRef inBundleID;
	FSRef outAppRef;
	UInt8 path[PATH_MAX];
	
	inCreator = (creator == Qnil) ? kLSUnknownCreator : rbStringToDescType(creator);
	if (bundleID != Qnil) {
		inBundleID = CFStringCreateWithBytes(NULL,
											 (UInt8 *)(RSTRING(bundleID)->ptr),
											 (CFIndex)(RSTRING(bundleID)->len),
											 kCFStringEncodingUTF8,
											 false);
		if (inBundleID == NULL) rb_raise(rb_eRuntimeError, "Invalid bundle ID string.");
	} else {
		inBundleID = NULL;
	}
	if (name != Qnil) {
		inName = CFStringCreateWithBytes(NULL,
										 (UInt8 *)(RSTRING(name)->ptr),
										 (CFIndex)(RSTRING(name)->len),
										 kCFStringEncodingUTF8,
										 false);
		if (inName == NULL) {
			if (inBundleID != NULL) CFRelease(inBundleID);
			rb_raise(rb_eRuntimeError, "Invalid name string.");
		}
	} else {
		inName = NULL;
	}
	err = LSFindApplicationForInfo(inCreator,
								   inBundleID,
								   inName,
								   &outAppRef,
								   NULL);
	if (inBundleID != NULL) CFRelease(inBundleID);
	if (inName != NULL) CFRelease(inName);
	if (err != 0) rbAE_raiseMacOSError("Couldn't find application.", err);
	err = FSRefMakePath(&outAppRef, path, PATH_MAX);
	if (err != 0) rbAE_raiseMacOSError("Couldn't get application path.", err);
	return rb_str_new2((char *)path);
}


static VALUE
rbAE_psnForApplicationPath(VALUE self, VALUE path)
{
	OSStatus err = noErr;
	ProcessSerialNumber psn = {0, kNoProcess};
	FSRef appRef, foundRef;

	err = FSPathMakeRef((UInt8 *)StringValuePtr(path), &appRef, NULL);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make FSRef for application.", err);
	while (1) {
		err = GetNextProcess(&psn);
		if (err != 0) rbAE_raiseMacOSError("Can't get next process.", err); // -600 if no more processes left
		err = GetProcessBundleLocation(&psn, &foundRef);
		if (err != 0) continue;
		if (FSCompareFSRefs(&appRef, &foundRef) == noErr) 
				return rb_ary_new3(2, INT2NUM(psn.highLongOfPSN), INT2NUM(psn.lowLongOfPSN));
	}
}


static VALUE
rbAE_launchApplication(VALUE self, VALUE path, VALUE firstEvent, VALUE flags)
{
	FSRef appRef;
	FSSpec fss;
	AEDesc paraDesc;
	Size paraSize;
	AppParametersPtr paraData;
	ProcessSerialNumber psn;
	LaunchParamBlockRec launchParams;
	OSErr err = noErr;
	
	err = FSPathMakeRef((UInt8 *)StringValuePtr(path), &appRef, NULL);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make FSRef for application.", err);
	err = FSGetCatalogInfo(&appRef, kFSCatInfoNone, NULL, NULL, &fss, NULL);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make FSSpec for application.", err);
	err = AECoerceDesc(&(AEDESC_OF(firstEvent)), typeAppParameters, &paraDesc);
	paraSize = AEGetDescDataSize(&paraDesc);
	paraData = (AppParametersPtr)NewPtr(paraSize);
	if (paraData == NULL) rbAE_raiseMacOSError("Can't make app parameters AEDesc.", memFullErr);
	err = AEGetDescData(&paraDesc, paraData, paraSize);
	if (err != noErr) rbAE_raiseMacOSError("Can't get AEDesc data.", err);
	launchParams.launchBlockID = extendedBlock;
	launchParams.launchEPBLength = extendedBlockLen;
	launchParams.launchFileFlags = 0;
	launchParams.launchControlFlags = (LaunchFlags)NUM2UINT(flags);
	launchParams.launchAppSpec = &fss;
	launchParams.launchAppParameters = paraData;
	err = LaunchApplication(&launchParams);
	if (err != noErr) rbAE_raiseMacOSError("Can't launch application.", err);
	psn = launchParams.launchProcessSN;
	return rb_ary_new3(2, INT2NUM(psn.highLongOfPSN), INT2NUM(psn.lowLongOfPSN));
}

static VALUE
rbAE_pidToPsn(VALUE self, VALUE pid)
{
	OSStatus err = 0;
	ProcessSerialNumber psn;
	
	err = GetProcessForPID((pid_t)NUM2INT(pid), &psn);
	if (err != noErr) rbAE_raiseMacOSError("Can't convert PID to PSN.", err);
	return rb_ary_new3(2, INT2NUM(psn.highLongOfPSN), INT2NUM(psn.lowLongOfPSN));
}

/**********************************************************************/
// Date conversion

static VALUE
rbAE_convertLongDateTimeToUnixSeconds(VALUE self, VALUE ldt)
{
	OSStatus err = 0;
	CFAbsoluteTime cfTime;
	
	err = UCConvertLongDateTimeToCFAbsoluteTime(rb_big2ll(ldt), &cfTime);
	if (err != noErr) rbAE_raiseMacOSError("Can't convert LongDateTime to seconds.", err);
	return rb_float_new(cfTime + kCFAbsoluteTimeIntervalSince1970);
}


static VALUE
rbAE_convertUnixSecondsToLongDateTime(VALUE self, VALUE secs)
{
	OSStatus err = 0;
	SInt64 ldt;

	err = UCConvertCFAbsoluteTimeToLongDateTime(NUM2DBL(secs) - kCFAbsoluteTimeIntervalSince1970, &ldt);
	if (err != noErr) rbAE_raiseMacOSError("Can't convert seconds to LongDateTime.", err);
	return rb_ll2big(ldt);
}


/**********************************************************************/
// Get aete

static VALUE
rbAE_OSAGetAppTerminology(VALUE self, VALUE path)
{
	FSRef appRef;
	FSSpec fss;
	ComponentInstance defaultComponent;
	Boolean didLaunch;
	AEDesc theDesc;
	OSErr err = noErr;
	
	err = FSPathMakeRef((UInt8 *)StringValuePtr(path), &appRef, NULL);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make FSRef.", err);
	err = FSGetCatalogInfo(&appRef, kFSCatInfoNone, NULL, NULL, &fss, NULL);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make FSSpec.", err);
	defaultComponent = OpenDefaultComponent(kOSAComponentType, 'ascr');
	err = GetComponentInstanceError(defaultComponent);
	if (err != 0) rbAE_raiseMacOSError("Couldn't make default component instance.", err);
	err = OSAGetAppTerminology(defaultComponent, 
							   kOSAModeNull,
							   &fss, 
							   0,
							   &didLaunch, 
							   &theDesc);
	if (err != 0) rbAE_raiseMacOSError("Couldn't get aete resource.", err);
	return rbAE_wrapAEDesc(&theDesc);
}


/**********************************************************************/
// Install event handlers

// Based on Python's CarbonX.AE extension

// TO DO: make sure GC won't collect handler objects while they're installed as event/coercion handlers

static pascal OSErr
rbAE_GenericEventHandler(const AppleEvent *request, AppleEvent *reply, refcontype refcon)
{
	VALUE err;
	
	err = rb_funcall((VALUE)refcon, 
					 rb_intern("handle_event"), 
					 2, 
					 rbAE_wrapBorrowedAEDesc(request),
					 rbAE_wrapBorrowedAEDesc(reply));
	return NUM2INT(err);
}

/*******/

static VALUE
rbAE_AEInstallEventHandler(VALUE self, VALUE eventClass, VALUE eventID, VALUE handler)
{
	/* 
	 * eventClass and eventID must be four-character code strings
	 *
	 * handler must be a Ruby object containing a method named 'handle_event' that takes two
	 * AppleEvent descriptors (request and reply) as arguments, and returns an integer.
	 * Note that this object is responsible for trapping any unhandled exceptions and returning
	 * an OS error number as appropriate (or 0 if no error), otherwise the program will exit.
	 */
	OSErr err = noErr;
	
	err = AEInstallEventHandler(rbStringToDescType(eventClass),
								rbStringToDescType(eventID),
	                            upp_GenericEventHandler, (long)handler,
	                            0);
	if (err != noErr) rbAE_raiseMacOSError("Can't install event handler.", err);
	return Qnil;
}


static VALUE
rbAE_AERemoveEventHandler(VALUE self, VALUE eventClass, VALUE eventID)
{
	OSErr err = noErr;
	
	err = AERemoveEventHandler(rbStringToDescType(eventClass),
							   rbStringToDescType(eventID),
							   upp_GenericEventHandler,
							   0);
	if (err != noErr) rbAE_raiseMacOSError("Can't remove event handler.", err);
	return Qnil;
}


static VALUE
rbAE_AEGetEventHandler(VALUE self, VALUE eventClass, VALUE eventID)
{
	OSErr err = noErr;
	AEEventHandlerUPP handlerUPP;
	VALUE handler;
	
	err = AEGetEventHandler(rbStringToDescType(eventClass),
							 rbStringToDescType(eventID),
	                         &handlerUPP, (long *)&handler,
	                         0);
	if (err != noErr) rbAE_raiseMacOSError("Can't get event handler.", err);
	return handler;
}


/**********************************************************************/
// Install coercion handlers

// TO DO: make sure GC won't collect handler objects while they're installed as event/coercion handlers

static pascal OSErr
rbAE_GenericCoercionHandler(const AEDesc *fromDesc, DescType toType, refcontype refcon, AEDesc *toDesc)
{
	// handle_coercion method should return an AE::AEDesc, or nil if an error occurred
	OSErr err = noErr;
	VALUE res;
	
	res = rb_funcall((VALUE)refcon, 
					 rb_intern("handle_coercion"),
					 2,
					 rbAE_wrapBorrowedAEDesc(fromDesc),
					 rbDescTypeToString(toType));
	if (rb_obj_is_instance_of(res, cAEDesc) != Qtrue) return errAECoercionFail;
	err = AEDuplicateDesc(&AEDESC_OF(res), toDesc);
	return err;
}


/*******/

static VALUE
rbAE_AEInstallCoercionHandler(VALUE self, VALUE fromType, VALUE toType, VALUE handler)
{
	/* 
	 * fromType and toType must be four-character code strings
	 *
	 * handler must be a Ruby object containing a method named 'handle_coercion' that takes an
	 * AEDesc and a four-character code (original value, desired type) as arguments, and returns an
	 * AEDesc of the desired type.Note that this object is responsible for trapping any unhandled 
	 * exceptions and returning nil (or any other non-AEDesc value) as appropriate, otherwise the 
	 * program will exit.
	 */
	OSErr err = noErr;
	
	err = AEInstallCoercionHandler(rbStringToDescType(fromType),
								   rbStringToDescType(toType),
								   upp_GenericCoercionHandler, (long)handler,
								   1, 0);
	if (err != noErr) rbAE_raiseMacOSError("Can't install coercion handler.", err);
	return Qnil;
}


static VALUE
rbAE_AERemoveCoercionHandler(VALUE self, VALUE fromType, VALUE toType)
{
	OSErr err = noErr;
	
	err = AERemoveCoercionHandler(rbStringToDescType(fromType),
								  rbStringToDescType(toType),
								  upp_GenericCoercionHandler,
								  0);
	if (err != noErr) rbAE_raiseMacOSError("Can't remove coercion handler.", err);
	return Qnil;
}


static VALUE
rbAE_AEGetCoercionHandler(VALUE self, VALUE fromType, VALUE toType)
{
	OSErr err = noErr;
	AECoercionHandlerUPP handlerUPP;
	VALUE handler;
	Boolean fromTypeIsDesc;
	
	err = AEGetCoercionHandler(rbStringToDescType(fromType),
							   rbStringToDescType(toType),
							   &handlerUPP, (long *)&handler,
							   &fromTypeIsDesc,
							   0);
	if (err != noErr) rbAE_raiseMacOSError("Can't get coercion handler.", err);
	return rb_ary_new3(2, handler, fromTypeIsDesc ? Qtrue : Qfalse);
}



		
/**********************************************************************/
// Process management

static VALUE
rbAE_RunApplicationEventLoop(VALUE self)
{
	RunApplicationEventLoop();
	return Qnil;
}

static VALUE
rbAE_QuitApplicationEventLoop(VALUE self)
{
	QuitApplicationEventLoop();
	return Qnil;
}

static VALUE
rbAE_transformProcessToForegroundApplication(VALUE self)
{
	OSStatus err = 0;
	ProcessSerialNumber psn = {0, kCurrentProcess};
	
	err = TransformProcessType(& psn, kProcessTransformToForegroundApplication);
	if( err != 0) rbAE_raiseMacOSError("Can't transform process.", err);
	return Qnil;
}

		
/**********************************************************************/
// Initialisation

void
Init_ae (void)
{

	mAE = rb_define_module("AE");

	// AE::AEDesc
	
	cAEDesc = rb_define_class_under(mAE, "AEDesc", rb_cObject);
	
	rb_define_singleton_method(cAEDesc, "new", rbAE_AEDesc_new, 2);
	rb_define_singleton_method(cAEDesc, "new_list", rbAE_AEDesc_newList, 1);
	rb_define_singleton_method(cAEDesc, "new_apple_event", rbAE_AEDesc_newAppleEvent, 5);
	
	rb_define_method(cAEDesc, "to_s", rbAE_AEDesc_inspect, 0);
	rb_define_method(cAEDesc, "inspect", rbAE_AEDesc_inspect, 0);
	rb_define_method(cAEDesc, "type", rbAE_AEDesc_type, 0);
	rb_define_method(cAEDesc, "data", rbAE_AEDesc_data, 0);
	rb_define_method(cAEDesc, "is_record?", rbAE_AEDesc_isRecord, 0);
	rb_define_method(cAEDesc, "coerce", rbAE_AEDesc_coerce, 1);
	rb_define_method(cAEDesc, "length", rbAE_AEDesc_length, 0);
	rb_define_method(cAEDesc, "put_item", rbAE_AEDesc_putItem, 2);
	rb_define_method(cAEDesc, "put_param", rbAE_AEDesc_putParam, 2);
	rb_define_method(cAEDesc, "put_attr", rbAE_AEDesc_putAttr, 2);
	rb_define_method(cAEDesc, "get", rbAE_AEDesc_get, 2);
	rb_define_method(cAEDesc, "send", rbAE_AEDesc_send, 2);
	
	// AE::MacOSError
	
	cMacOSError = rb_define_class_under(mAE, "MacOSError", rb_eStandardError);
	
	rb_define_attr(cMacOSError, "number", Qtrue, Qfalse);
	rb_define_attr(cMacOSError, "description", Qtrue, Qfalse);
	
	rb_define_alias(cMacOSError, "to_i", "number");
	
	rb_define_method(cMacOSError, "to_s", rbAE_MacOSError_inspect, 0);
	rb_define_method(cMacOSError, "inspect", rbAE_MacOSError_inspect, 0);

	// Support functions
	
	rb_define_module_function(mAE, "find_application", rbAE_findApplication, 3);
	rb_define_module_function(mAE, "psn_for_application_path", rbAE_psnForApplicationPath, 1);
	rb_define_module_function(mAE, "launch_application", rbAE_launchApplication, 3);
	rb_define_module_function(mAE, "pid_to_psn", rbAE_pidToPsn, 1);

	rb_define_module_function(mAE, "convert_long_date_time_to_unix_seconds", 
							  rbAE_convertLongDateTimeToUnixSeconds, 1);
	rb_define_module_function(mAE, "convert_unix_seconds_to_long_date_time", 
							  rbAE_convertUnixSecondsToLongDateTime, 1);
							  
	rb_define_module_function(mAE, "get_app_terminology", rbAE_OSAGetAppTerminology, 1);
	
	// Event handling
	
	upp_GenericEventHandler = NewAEEventHandlerUPP(rbAE_GenericEventHandler);
	upp_GenericCoercionHandler = NewAECoerceDescUPP(rbAE_GenericCoercionHandler);
	
	rb_define_module_function(mAE, "install_event_handler", rbAE_AEInstallEventHandler, 3);
	rb_define_module_function(mAE, "remove_event_handler", rbAE_AERemoveEventHandler, 2);
	rb_define_module_function(mAE, "get_event_handler", rbAE_AEGetEventHandler, 2);
	
	rb_define_module_function(mAE, "install_coercion_handler", rbAE_AEInstallCoercionHandler, 3);
	rb_define_module_function(mAE, "remove_coercion_handler", rbAE_AERemoveCoercionHandler, 2);
	rb_define_module_function(mAE, "get_coercion_handler", rbAE_AEGetCoercionHandler, 2);
	
	rb_define_module_function(mAE, "run_application_event_loop", rbAE_RunApplicationEventLoop, 0);
	rb_define_module_function(mAE, "quit_application_event_loop", rbAE_QuitApplicationEventLoop, 0);
	rb_define_module_function(mAE, "transform_process_to_foreground_application", 
							  rbAE_transformProcessToForegroundApplication, 0);
}
