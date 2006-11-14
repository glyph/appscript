#!/usr/local/bin/ruby
# Copyright (C) 2006 HAS. 
# Released under MIT License.

# Note: unit type names have been altered to match built-in unit types in codecs.rb

module DefaultTerminology

	require "aem"

	TypeByCode = {
		'mile' => :miles,
		'cyrd' => :cubic_yards,
		'sqkm' => :square_kilometers,
		'kmtr' => :kilometers,
		'gram' => :grams,
		'metr' => :meters,
		'sqmi' => :square_miles,
		'ccmt' => :cubic_centimeter,
		'cmet' => :cubic_meters,
		'feet' => :feet,
		'cmtr' => :centimeters,
		'yard' => :yards,
		'sqft' => :square_feet,
		'qrts' => :quarts,
		'ozs ' => :ounces,
		'galn' => :gallons,
		'cuin' => :cubic_inches,
		'kgrm' => :kilograms,
		'litr' => :liters,
		'sqrm' => :square_meters,
		'inch' => :inches,
		'sqyd' => :square_yards,
		'cfet' => :cubic_feet,
		'lbs ' => :pounds,
		'degc' => :Celsius,
		'degf' => :Fahrenheit,
		'degk' => :Kelvin,
		
		'jan ' => :January,
		'feb ' => :February,
		'mar ' => :March,
		'apr ' => :April,
		'may ' => :May,
		'jun ' => :June,
		'jul ' => :July,
		'aug ' => :August,
		'sep ' => :September,
		'oct ' => :October,
		'nov ' => :November,
		'dec ' => :December,
		'sun ' => :Sunday,
		'mon ' => :Monday,
		'tue ' => :Tuesday,
		'wed ' => :Wednesday,
		'thu ' => :Thursday,
		'fri ' => :Friday,
		'sat ' => :Saturday,
		
		'decm' => :DecimalStruct,
		'shor' => :ShortInteger,
		'TEXT' => :String,
		'elin' => :ElemInfo,
		'cmpd' => :CompDescriptor,
		'cmpi' => :ComponentInstance,
		'prop' => :Property,
		'trot' => :Rotation,
		'pstr' => :PString,
		'PICT' => :Pict,
		'crec' => :Rectangle,
		'cpic' => :GroupedGraphic,
		'vers' => :Version,
		'fals' => :False,
		'obj ' => :Reference,
		'fwin' => :FinderWindow,
		'frct' => :FixedRectangle,
		'cpix' => :PixelMap,
		'aevt' => :AppleEvent,
		'evrf' => :EventRef,
		'cRGB' => :RGBColor,
		'cgtx' => :GraphicText,
		'list' => :List,
		'fsrf' => :FSRef,
		'exte' => :Extended,
		'ctbl' => :Table,
		'ccol' => :Column,
		'tray' => :TextRangeArray,
		'tr96' => :RGB96,
		'fpnt' => :FixedPoint,
		'crow' => :Row,
		'sign' => :ApplSignature,
		'tTXT' => :AEText,
		'doub' => :Float,
		'tsty' => :TextStyles,
		'encs' => :EncodedString,
		'whit' => :whitespace,
		'keyw' => :Keyword,
		'best' => :Best,
		'logi' => :LogicalDescriptor,
		'ssid' => :SessionID,
		'****' => :Anything,
		'diac' => :diacriticals,
		'cpgn' => :Polygon,
		'yes ' => :yes,
		'txrn' => :TextRange,
		'psn ' => :ProcessSerialNumber,
		'comp' => :DoubleInteger,
		'hyph' => :hyphens,
		'reco' => :Record,
		'Qrgn' => :QDRegion,
		'evrc' => :EventRecord,
		'carc' => :Arc,
		'enum' => :Enumeration,
		'QDpt' => :QDPoint,
		'magn' => :UnsignedInteger,
		'ccel' => :Cell,
		'fixd' => :Fixed,
		'abso' => :AbsoluteOrdinal,
		'punc' => :punctuation,
		'lpnt' => :LongPoint,
		'gcli' => :ClassInfo,
		'tdas' => :DashStyle,
		'ldbl' => :K128BitFloatingPoint,
		'rang' => :RangeDescriptor,
		'intl' => :IntlWritingCode,
		'cdrw' => :DrawingArea,
		'STXT' => :StyledText,
		'itxt' => :IntlText,
		'GIFf' => :GIF,
		'sing' => :ShortFloat,
		'ldt ' => :DateTime,
		'evin' => :EventInfo,
		'msng' => :MissingValue,
		'wrng' => :WhoseRange,
		'lfrc' => :LongFixedRectangle,
		'TIFF' => :TIFF,
		'suin' => :SuiteInfo,
		'long' => :Integer,
		'whos' => :WhoseDescriptor,
		'pmin' => :ParamInfo,
		'lfpt' => :LongFixedPoint,
		'dspt' => :DispatcherID,
		'lfxd' => :LongFixed,
		'type' => :Type,
		'appa' => :AppParameters,
		'tr16' => :RGB16,
		'evtr' => :LowLevelEventRecord,
		'glin' => :GraphicLine,
		'crrc' => :RoundedRectangle,
		'wobj' => :HIWindow,
		'true' => :True,
		'case' => :case,
		'utxt' => :Unicode,
		'styl' => :ScrapStyles,
		'lrct' => :LongRectangle,
		'ask ' => :ask,
		'expa' => :expansion,
		'alis' => :Alias,
		'clrt' => :ColorTable,
		'aprl' => :ApplicationURL,
		'mLoc' => :MachineLoc,
		'EPS ' => :EPS,
		'tpmm' => :PixMapMinus,
		'null' => :Null,
		'ccnt' => :CurrentContainer,
		'sutx' => :StyledUnicodeText,
		'furl' => :FileURL,
		'scpt' => :Script,
		'rel ' => :RelativeDescriptor,
		'targ' => :TargetID,
		'pinf' => :PropInfo,
		'bool' => :Boolean,
		'qdrt' => :QDRectangle,
		'pcls' => :class_,
		'ostl' => :OSLTokenList,
		'covl' => :Oval,
		'no  ' => :no,
		'insl' => :InsertionRef,
		'JPEG' => :JPEG,
		'exmn' => :ObjectBeingExamined,
		'cstr' => :CString,
		'ofay' => :OffsetArray,
		'mobj' => :HIMenu,
		'toke' => :Token,
		'inde' => :IndexDescriptor,
		'tdta' => :Data,
		'fss ' => :FSSpec,
		'sect' => :SectionH,
		'ptr ' => :Ptr,
	}

	TypeByName = {
		:square_miles => AEM::AEType.new('sqmi'),
		:cubic_meters => AEM::AEType.new('cmet'),
		:kilograms => AEM::AEType.new('kgrm'),
		:square_kilometers => AEM::AEType.new('sqkm'),
		:yards => AEM::AEType.new('yard'),
		:centimeters => AEM::AEType.new('cmtr'),
		:inches => AEM::AEType.new('inch'),
		:grams => AEM::AEType.new('gram'),
		:cubic_inches => AEM::AEType.new('cuin'),
		:meters => AEM::AEType.new('metr'),
		:square_meters => AEM::AEType.new('sqrm'),
		:kilometers => AEM::AEType.new('kmtr'),
		:cubic_yards => AEM::AEType.new('cyrd'),
		:square_yards => AEM::AEType.new('sqyd'),
		:quarts => AEM::AEType.new('qrts'),
		:feet => AEM::AEType.new('feet'),
		:ounces => AEM::AEType.new('ozs '),
		:cubic_feet => AEM::AEType.new('cfet'),
		:liters => AEM::AEType.new('litr'),
		:cubic_centimeter => AEM::AEType.new('ccmt'),
		:square_feet => AEM::AEType.new('sqft'),
		:pounds => AEM::AEType.new('lbs '),
		:gallons => AEM::AEType.new('galn'),
		:miles => AEM::AEType.new('mile'),
		:Celsius => AEM::AEType.new('degc'),
		:Fahrenheit => AEM::AEType.new('degf'),
		:Kelvin => AEM::AEType.new('degk'),
		
		:January => AEM::AEType.new('jan '),
		:February => AEM::AEType.new('feb '),
		:March => AEM::AEType.new('mar '),
		:April => AEM::AEType.new('apr '),
		:May => AEM::AEType.new('may '),
		:June => AEM::AEType.new('jun '),
		:July => AEM::AEType.new('jul '),
		:August => AEM::AEType.new('aug '),
		:September => AEM::AEType.new('sep '),
		:October => AEM::AEType.new('oct '),
		:November => AEM::AEType.new('nov '),
		:December => AEM::AEType.new('dec '),
		:Sunday => AEM::AEType.new('sun '),
		:Monday => AEM::AEType.new('mon '),
		:Tuesday => AEM::AEType.new('tue '),
		:Wednesday => AEM::AEType.new('wed '),
		:Thursday => AEM::AEType.new('thu '),
		:Friday => AEM::AEType.new('fri '),
		:Saturday => AEM::AEType.new('sat '),
		
		:ObjectSpecifier => AEM::AEType.new('obj '),
		:SMInt => AEM::AEType.new('shor'),
		:PropInfo => AEM::AEType.new('pinf'),
		:InsertionRef => AEM::AEType.new('insl'),
		:EPS => AEM::AEType.new('EPS '),
		:DateTime => AEM::AEType.new('ldt '),
		:HIWindow => AEM::AEType.new('wobj'),
		:LongDateTime => AEM::AEType.new('ldt '),
		:ColorTable => AEM::AEType.new('clrt'),
		:yes => AEM::AEEnum.new('yes '),
		:LowLevelEventRecord => AEM::AEType.new('evtr'),
		:OffsetArray => AEM::AEType.new('ofay'),
		:ClassInfo => AEM::AEType.new('gcli'),
		:SInt64 => AEM::AEType.new('comp'),
		:SessionID => AEM::AEType.new('ssid'),
		:PixMapMinus => AEM::AEType.new('tpmm'),
		:ScrapStyles => AEM::AEType.new('styl'),
		:IntlText => AEM::AEType.new('itxt'),
		:AbsoluteOrdinal => AEM::AEType.new('abso'),
		:RangeDescriptor => AEM::AEType.new('rang'),
		:ShortFloat => AEM::AEType.new('sing'),
		:Keyword => AEM::AEType.new('keyw'),
		:FSSpec => AEM::AEType.new('fss '),
		:hyphens => AEM::AEEnum.new('hyph'),
		:GIF => AEM::AEType.new('GIFf'),
		:Token => AEM::AEType.new('toke'),
		:Fixed => AEM::AEType.new('fixd'),
		:SMFloat => AEM::AEType.new('sing'),
		:WhoseDescriptor => AEM::AEType.new('whos'),
		:ApplSignature => AEM::AEType.new('sign'),
		:False => AEM::AEType.new('fals'),
		:Reference => AEM::AEType.new('obj '),
		:CurrentContainer => AEM::AEType.new('ccnt'),
		:True => AEM::AEType.new('true'),
		:ElemInfo => AEM::AEType.new('elin'),
		:Arc => AEM::AEType.new('carc'),
		:EventRecord => AEM::AEType.new('evrc'),
		:GroupedGraphic => AEM::AEType.new('cpic'),
		:AppParameters => AEM::AEType.new('appa'),
		:Script => AEM::AEType.new('scpt'),
		:AEList => AEM::AEType.new('list'),
		:Enumeration => AEM::AEType.new('enum'),
		:FinderWindow => AEM::AEType.new('fwin'),
		:GraphicLine => AEM::AEType.new('glin'),
		:AppleEvent => AEM::AEType.new('aevt'),
		:Version => AEM::AEType.new('vers'),
		:Type => AEM::AEType.new('type'),
		:LongFloat => AEM::AEType.new('doub'),
		:FSRef => AEM::AEType.new('fsrf'),
		:whitespace => AEM::AEEnum.new('whit'),
		:SectionH => AEM::AEType.new('sect'),
		:LongPoint => AEM::AEType.new('lpnt'),
		:WildCard => AEM::AEType.new('****'),
		:LongFixedPoint => AEM::AEType.new('lfpt'),
		:Rotation => AEM::AEType.new('trot'),
		:QDPoint => AEM::AEType.new('QDpt'),
		:MissingValue => AEM::AEType.new('msng'),
		:ObjectBeingExamined => AEM::AEType.new('exmn'),
		:Unicode => AEM::AEType.new('utxt'),
		:QDRectangle => AEM::AEType.new('qdrt'),
		:TIFF => AEM::AEType.new('TIFF'),
		:ApplicationURL => AEM::AEType.new('aprl'),
		:expansion => AEM::AEEnum.new('expa'),
		:Record => AEM::AEType.new('reco'),
		:AERecord => AEM::AEType.new('reco'),
		:FixedRectangle => AEM::AEType.new('frct'),
		:ask => AEM::AEEnum.new('ask '),
		:FileURL => AEM::AEType.new('furl'),
		:Property => AEM::AEType.new('prop'),
		:AEText => AEM::AEType.new('tTXT'),
		:IndexDescriptor => AEM::AEType.new('inde'),
		:DoubleInteger => AEM::AEType.new('comp'),
		:EventInfo => AEM::AEType.new('evin'),
		:class_ => AEM::AEType.new('pcls'),
		:SuiteInfo => AEM::AEType.new('suin'),
		:Char => AEM::AEType.new('TEXT'),
		:StyledUnicodeText => AEM::AEType.new('sutx'),
		:IEEE64BitFloatingPoint => AEM::AEType.new('doub'),
		:Null => AEM::AEType.new('null'),
		:EncodedString => AEM::AEType.new('encs'),
		:Ptr => AEM::AEType.new('ptr '),
		:Best => AEM::AEType.new('best'),
		:Row => AEM::AEType.new('crow'),
		:GraphicText => AEM::AEType.new('cgtx'),
		:ParamInfo => AEM::AEType.new('pmin'),
		:TextRangeArray => AEM::AEType.new('tray'),
		:LongFixedRectangle => AEM::AEType.new('lfrc'),
		:Cell => AEM::AEType.new('ccel'),
		:diacriticals => AEM::AEEnum.new('diac'),
		:FSS => AEM::AEType.new('fss '),
		:Enumerated => AEM::AEType.new('enum'),
		:LongInteger => AEM::AEType.new('long'),
		:TextRange => AEM::AEType.new('txrn'),
		:Rectangle => AEM::AEType.new('crec'),
		:Extended => AEM::AEType.new('exte'),
		:ProcessSerialNumber => AEM::AEType.new('psn '),
		:ShortInteger => AEM::AEType.new('shor'),
		:Comp => AEM::AEType.new('comp'),
		:HIMenu => AEM::AEType.new('mobj'),
		:UnicodeText => AEM::AEType.new('utxt'),
		:LongFixed => AEM::AEType.new('lfxd'),
		:Integer => AEM::AEType.new('long'),
		:Data => AEM::AEType.new('tdta'),
		:RelativeDescriptor => AEM::AEType.new('rel '),
		:DashStyle => AEM::AEType.new('tdas'),
		:Column => AEM::AEType.new('ccol'),
		:IEEE32BitFloatingPoint => AEM::AEType.new('sing'),
		:Magnitude => AEM::AEType.new('magn'),
		:QDRegion => AEM::AEType.new('Qrgn'),
		:IntlWritingCode => AEM::AEType.new('intl'),
		:DecimalStruct => AEM::AEType.new('decm'),
		:case => AEM::AEEnum.new('case'),
		:List => AEM::AEType.new('list'),
		:Text => AEM::AEType.new('TEXT'),
		:CString => AEM::AEType.new('cstr'),
		:RGBColor => AEM::AEType.new('cRGB'),
		:UInt32 => AEM::AEType.new('magn'),
		:PixelMap => AEM::AEType.new('cpix'),
		:Oval => AEM::AEType.new('covl'),
		:Anything => AEM::AEType.new('****'),
		:SInt32 => AEM::AEType.new('long'),
		:Polygon => AEM::AEType.new('cpgn'),
		:TargetID => AEM::AEType.new('targ'),
		:InsertionLoc => AEM::AEType.new('insl'),
		:LogicalDescriptor => AEM::AEType.new('logi'),
		:StyledText => AEM::AEType.new('STXT'),
		:no => AEM::AEEnum.new('no  '),
		:punctuation => AEM::AEEnum.new('punc'),
		:LongRectangle => AEM::AEType.new('lrct'),
		:Pict => AEM::AEType.new('PICT'),
		:MachineLoc => AEM::AEType.new('mLoc'),
		:OSLTokenList => AEM::AEType.new('ostl'),
		:FixedPoint => AEM::AEType.new('fpnt'),
		:RGB16 => AEM::AEType.new('tr16'),
		:DrawingArea => AEM::AEType.new('cdrw'),
		:ComponentInstance => AEM::AEType.new('cmpi'),
		:TextStyles => AEM::AEType.new('tsty'),
		:UnsignedInteger => AEM::AEType.new('magn'),
		:DispatcherID => AEM::AEType.new('dspt'),
		:JPEG => AEM::AEType.new('JPEG'),
		:EventRef => AEM::AEType.new('evrf'),
		:Alias => AEM::AEType.new('alis'),
		:PString => AEM::AEType.new('pstr'),
		:RoundedRectangle => AEM::AEType.new('crrc'),
		:String => AEM::AEType.new('TEXT'),
		:SInt16 => AEM::AEType.new('shor'),
		:WhoseRange => AEM::AEType.new('wrng'),
		:Float => AEM::AEType.new('doub'),
		:CompDescriptor => AEM::AEType.new('cmpd'),
		:RGB96 => AEM::AEType.new('tr96'),
		:Boolean => AEM::AEType.new('bool'),
		:Table => AEM::AEType.new('ctbl'),
		:K128BitFloatingPoint => AEM::AEType.new('ldbl'),
	}

	ReferenceByCode = {
		'pcls' => 'class_',
		'ID  ' => 'id_',
	}
	
	ReferenceByName = {
		:quit => [:command, ['aevtquit', {
				:saving => 'savo',
				}]],
		:activate => [:command, ['miscactv', {
				}]],
		:run => [:command, ['aevtoapp', {
				}]],
		:launch => [:command, ['ascrnoop', {
				}]],
		:open => [:command, ['aevtodoc', {
				}]],
		:get => [:command, ['coregetd', {
				}]],
		:print => [:command, ['aevtpdoc', {
				}]],
		:class_ => [:property, 'pcls'],
		:set => [:command, ['coresetd', {
				:to => 'data',
				}]],
		:reopen => [:command, ['aevtrapp', {
				}]],
		:id_ => [:property, 'ID  '],
		:open_location => [:command, ['GURLGURL', {
				:window => 'WIND',
				}]],
	}

	DefaultCommands = {} # {'quit' => 'aevtquit', 'activate' => 'miscactv',...}; used by Terminology._make_reference_table to check for any collisions between standard and application-defined commands
	
	ReferenceByName.each do |name, info|
		if info[0] == :command
			DefaultCommands[name.to_s] = info[1][0]
		end
	end
end
