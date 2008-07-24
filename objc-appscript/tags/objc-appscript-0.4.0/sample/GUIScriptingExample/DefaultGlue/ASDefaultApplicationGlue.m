/*
 * ASDefaultApplicationGlue.m
 *
 * <default terminology>
 * osaglue 0.3.2
 *
 */

#import "ASDefaultApplicationGlue.h"



@implementation ASDefaultApplication

// clients shouldn't need to call this next method themselves
- (id)initWithTargetType:(ASTargetType)targetType_ data:(id)targetData_ {
    ASAppData *appData;
    
    appData = [[ASAppData alloc] initWithApplicationClass: [AEMApplication class]
                                            constantClass: [ASDefaultConstant class]
                                           referenceClass: [ASDefaultReference class]
                                               targetType: targetType_
                                               targetData: targetData_];
    self = [super initWithAppData: appData aemReference: AEMApp];
    if (!self) return self;
    return self;
}

// initialisers

- (id)init {
    return [self initWithTargetType: kASTargetCurrent data: nil];
}

- (id)initWithName:(NSString *)name {
    return [self initWithTargetType: kASTargetName data: name];
}

- (id)initWithBundleID:(NSString *)bundleID {
    return [self initWithTargetType: kASTargetBundleID data: bundleID];    
}

- (id)initWithURL:(NSURL *)url {
    return [self initWithTargetType: kASTargetURL data: url];
}

- (id)initWithPID:(pid_t)pid {
    return [self initWithTargetType: kASTargetPID data: [NSNumber numberWithUnsignedLong: pid]];
}

- (id)initWithDescriptor:(NSAppleEventDescriptor *)desc {
    return [self initWithTargetType: kASTargetDescriptor data: desc];
}

// misc

- (ASDefaultReference *)AS_newReferenceWithObject:(id)object {
	if ([object isKindOfClass: [ASDefaultReference class]])
		return [[[ASDefaultReference alloc] initWithAppData: AS_appData
				aemReference: [object AS_aemReference]] autorelease];
	else if ([object isKindOfClass: [AEMQuery class]])
		return [[[ASDefaultReference alloc] initWithAppData: AS_appData
				aemReference: object] autorelease];
	else if (!object)
		return [[[ASDefaultReference alloc] initWithAppData: AS_appData
				aemReference: AEMApp] autorelease];
	else
		return [[[ASDefaultReference alloc] initWithAppData: AS_appData
				aemReference: AEMRoot(object)] autorelease];
}

@end

