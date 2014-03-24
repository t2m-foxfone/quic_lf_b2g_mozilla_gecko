//JRD_ZMTANG create on 2012-09-04
'use strict';

//static functions
let DEBUG = 1;
if (DEBUG)
  var debug = function(s) { dump('<TZM_LOG> -*- JrdExtension -*-: ' + s + '\n'); };
else
  var debug = function(s) {};

const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;

Cu.import('resource://gre/modules/XPCOMUtils.jsm');
Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://gre/modules/DOMRequestHelper.jsm');
Cu.import('resource://gre/modules/FileUtils.jsm');


//JRD_KLWANG add for CR:414530---begin
XPCOMUtils.defineLazyGetter(this, "libcutils", function () {
  Cu.import("resource://gre/modules/systemlibs.js");
  return libcutils;
});
//JRD_KLWANG add for CR:414530---end

XPCOMUtils.defineLazyServiceGetter(this, 'cpmm',
                                   '@mozilla.org/childprocessmessagemanager;1',
                                   'nsIMessageSender');

function jrdIDOMExtension() {
}

//[data]: object or string, number....
function exposeReadOnly(data) {
  if (typeof data === 'object') {
    let exposedProps = {};
    for (let i in data) {
      exposedProps[i] = 'r';
      exposeReadOnly(data[i]);
    }

    data.__exposedProps__ = exposedProps;
  }
  return data;
}

// dingp@tcl.com add for NVram access at 2013-12-31 01:28:53 PM
function exposeWrite(data) {
  debug('exposeWrite========================');
  if (typeof data === 'object') {
    let exposedProps = {};
    for (let i in data) {
      exposedProps[i] = 'w';
      exposeWrite(data[i]);
    }
    data.__exposedProps__ = exposedProps;
  }

  return data;
}

jrdIDOMExtension.prototype = {

  __proto__: DOMRequestIpcHelper.prototype,

  //For XPCOM/XPCONNECT
  classDescription: 'The JRD extension for DOM',
  classID: Components.ID('{bad6c492-42cc-4080-89bf-de2f5e3bf6b8}'),
  contractID: '@jrdcom.com/extension;1',
  classInfo: XPCOMUtils.generateCI(
              {classID: Components.ID('{bad6c492-42cc-4080-89bf-de2f5e3bf6b8}'),
              ontractID: '@jrdcom.com/extension;1',
              classDescription: 'JRDExtension',
              interfaces: [Ci.jrdIDOMExtension],
              flags: Ci.nsIClassInfo.DOM_OBJECT}),
  QueryInterface: XPCOMUtils.generateQI([Ci.jrdIDOMExtension, Ci.nsIDOMGlobalPropertyInitializer, Ci.nsIMessageListener, Ci.nsISupportsWeakReference, Ci.nsIObserver]),

  //Init is defined in nsIDOMGlobalPropertyInitializer
  init: function(aWindow) {
    debug('Initialized');

    //JRD_KLWANG add for CR:461169----Protect JrdExtension WebAPI
    let perm = Services.perms.testExactPermissionFromPrincipal(aWindow.document.nodePrincipal, "jrdextension");
    this._hasPrivileges = perm == Ci.nsIPermissionManager.ALLOW_ACTION;

    if (!this._hasPrivileges) {
      Cu.reportError("NO JRD EXTENSION PERMISSION FOR: " + aWindow.document.nodePrincipal.origin + "\n");
      return null;
    }

    this._window = aWindow;
    this.initDOMRequestHelper(aWindow, ['JrdSrv:Common:Return',
      'JrdSrv:KeyEvent:Return',
      'JrdSrv:HeadsetStatusChangedEvt:Return',
      'JrdSrv:RunProcess:Return',
      'JrdSrv:CopyFile:Return',
      'JrdSrv:GetFilesLastTime:Return',
      'JrdSrv:CreateDirOrFile:Return',
      'JrdSrv:UniversalCommand:Return',//jrd_tyin add for UniversalCommand.
      'JrdSrv:CheckIsCommandRunnig:Return',
      'JrdSrv:ReadNvitem:Return', //jrd_tanya add for read Nvitem.
      'JrdSrv:WriteNvitem:Return' //jrd_tanya add for read Nvitem.
      ]);

    this._onKeyEvent = null;
    this._requestInfo = {};
  },

  //Get the current time, the time format is 20121017133144(YearMonthDayHourMinuteSecond)
  get _currentDateTime() {
    let tempDate = new Date();
    let date_str = tempDate.toLocaleFormat('%Y%m%d_%H%M%S');
    return date_str;
  },

  //Get the process command
  _getRunStr: function(aProcessType) {
    let run_str;

    switch (aProcessType) {
      case 'bugreport':
        run_str = '/system/bin/bugreport';
        break;

      case 'qxdm':
        run_str = '/system/bin/diag_mdlog -f /storage/sdcard1/jrdlog/qxdm/diag.cfg';
        break;

      case 'logcat':
        run_str = '/system/bin/logcat -b main -b system -b radio -b events -v time';
        break;

      case 'dmesg':
        run_str = '/system/bin/cat /proc/kmsg';
        break;

      default:
        debug('_getRunStr  type error' + aProcessType);
        break;
    }
    debug('_getRunStr  run_str: ' + run_str);
    return run_str;
  },

  //Get the directory path
  _getDestDicPath: function(aStoreToDevice, aProcessType) {
    let directoryPath;

    if (aStoreToDevice === 'PHONE') {
      directoryPath = '/data/jrdlog/' + aProcessType + '/';
    }
    else { //SDCARD
      directoryPath = '/storage/sdcard1/jrdlog/' + aProcessType + '/';
    }
    debug('_getDestDicPath  directoryPath: ' + directoryPath);
    return directoryPath;
  },

  // Get the whole file path
  _getDestWholePath: function(aStoreToDevice, aProcessType) {
    let destStr = this._getDestDicPath(aStoreToDevice, aProcessType);

    destStr += aProcessType + '_' + this._currentDateTime;
    debug('_getDestWholePath  destStr: ' + destStr);
    return destStr;
  },

  // Handle the receive message of 'JrdSrv:GetFilesLastTime'
  _filesLastTimeMsgHdlr: function(aMsg) {
    let obj = aMsg.data;
    let request = this.takeRequest(aMsg.requestID);
    let data = {};

    for (let typeStr in obj) {
      data[typeStr] = {};
      if (obj[typeStr].time !== 0) {
        let tempDate = new Date(obj[typeStr].time);
        let timeStr = tempDate.toLocaleFormat('%Y-%m-%d,%H:%M');

        data[typeStr].timeStr = timeStr;
        //7: the length of '/sdcard', 11: the length of '/mnt/sdcard'
        if (obj[typeStr].location.substr(0, 7) === '/sdcard' ||
          obj[typeStr].location.substr(0, 11) === '/mnt/sdcard') {
          data[typeStr].location = 'sdcard';
        }
        else {
          data[typeStr].location = 'phone';
        }
      }
      else {
        data[typeStr].timeStr = '';
        data[typeStr].location = '';
      }
    }
    debug('_filesLastTimeMsgHdlr send data: ' + JSON.stringify(data));
    Services.DOMRequest.fireSuccess(request, exposeReadOnly(data));

    delete this._requestInfo[aMsg.requestID];
  },

  // Handle the receive message of 'JrdSrv:CreateDirOrFile'
  _createFileMsgHdlr: function(aMsg) {
    debug('_createFileMsgHdlr' + JSON.stringify(this._requestInfo));
    for (let requestId in this._requestInfo) {
      if (requestId === aMsg.requestID) {
        if (this._requestInfo[requestId].type === 'check') {
          debug('_createFileMsgHdlr send data: ' + JSON.stringify(aMsg.data));
          let request = this.takeRequest(aMsg.requestID);

          Services.DOMRequest.fireSuccess(request, exposeReadOnly(aMsg.data));
          delete this._requestInfo[aMsg.requestID];
        }
        else if (aMsg.data === 'EXIST' || aMsg.data === 'CREATE_SUCCESS') {
          if (this._requestInfo[requestId].type === 'tombstones') {
            this._copyTombstones(aMsg.requestID);
          }
          else if (this._requestInfo[requestId].type === 'qxdm') {
            this._copyDiagCfg(aMsg.requestID);
          }
          else {
            this._runProcess(aMsg.requestID);
          }
        }
        else {
          debug('_createFileMsgHdlr send data: ' + aMsg.data);
          let request = this.takeRequest(aMsg.requestID);
          let data = {};

          data.type = this._requestInfo[aMsg.requestID].type;
          data['errorInfo'] = 'Directory or file not exist or create failed';
          Services.DOMRequest.fireError(request, JSON.stringify(data));

          delete this._requestInfo[aMsg.requestID];
        }

        return;
      }
    }
  },

  // Handle the receive message of 'JrdSrv:RunProces' and 'JrdSrv:CopyFile'
  _processMsgHdlr: function(aMessage) {
    let aMsg = aMessage.data;

    if (aMsg.requestID) { //Avoid the uninit case
      let aProcessState = aMsg.data.processState;

      for (let requestId in this._requestInfo) {
        if (requestId === aMsg.requestID) {
          let type = this._requestInfo[requestId].type;
  
          debug('processState:'+ aProcessState + "  type: " + type);
          if (aProcessState === 'process-finished') {
            if('qxdm' === type && 'JrdSrv:CopyFile:Return' === aMessage.name) {
              this._runProcess(aMsg.requestID);
            }
            else {
              let aFilePathArray = [];
    
              aFilePathArray[0] = this._getDestDicPath('PHONE', type) + type + '*';
              aFilePathArray[1] = this._getDestDicPath('SDCARD', type) + type + '*';
              cpmm.sendAsyncMessage('JrdSrv:GetFilesLastTime', {aFilePathArray: aFilePathArray, requestID: aMsg.requestID});
            }
          }
          else if (aProcessState === 'process-running') {
            //Diag_log or logcat will not observe or callback anything when it is running.
            //So add the "process-running" case to handle it.
            //Gaia can perform something in the onsuccess function of the refer request.
            if (type === 'qxdm' || type === 'logcat' || type ==='dmesg') {
              let request = this.getRequest(aMsg.requestID);
              let data = {};
              data.type = type;
              data.info = 'running';
    
              Services.DOMRequest.fireSuccess(request, exposeReadOnly(data));
            }
          } 
          else { //process-failed
            let request = this.takeRequest(aMsg.requestID);
            let data = {};
            data.type = type;
            data['errorInfo'] = 'Run process failed';
            data['errorType'] = aMsg.data.errorType;
            debug('process-failed: ' + JSON.stringify(data));
            Services.DOMRequest.fireError(request, JSON.stringify(data));
    
            delete this._requestInfo[aMsg.requestID];
          }

          return;
        }
      }
    }
  },

  _fileRead: function(_file) {
		let file = Cc['@mozilla.org/file/local;1'].createInstance(Ci.nsILocalFile);
		file.initWithPath(_file);
		let fstream = Cc['@mozilla.org/network/file-input-stream;1'].
		              createInstance(Ci.nsIFileInputStream);
		fstream.init(file, -1, 0, Ci.nsIFileInputStream.DEFER_OPEN);
		let cstream = Cc['@mozilla.org/intl/converter-input-stream;1'].
		              createInstance(Ci.nsIConverterInputStream);
		cstream.init(fstream, 'UTF-8', 0, 0);
		let string = {};
		cstream.readString(-1, string);
		cstream.close();
		fstream.close();
		return string.value;
	},

  //--DOM API--: getSysInfo()
  getSysInfo: function(aParam) {
    debug('getSysInfo: ' + JSON.stringify(aParam));

    let request = this.createRequest();

    cpmm.sendAsyncMessage('JrdSrv:Common',
       {type: aParam, requestID: this.getRequestId(request)});

    return request;
  },

  _copyTombstones: function(requestId) {
    debug('_copyTombstones: ' + JSON.stringify(this._requestInfo[requestId].type));

    let sourcePath = '/data/tombstones';
    let aParam = this._requestInfo[requestId].type;
    let directoryPath = this._getDestDicPath(this._requestInfo[requestId].storeToDevice,
       this._requestInfo[requestId].type);
    let fileName = aParam + '_' + this._currentDateTime;

    cpmm.sendAsyncMessage('JrdSrv:CopyFile',
      {sourcePath: sourcePath, directoryPath: directoryPath, fileNewName: fileName, requestID: requestId});
  },

  _copyDiagCfg: function(requestId) {
    debug('_copyTombstones: ' + JSON.stringify(this._requestInfo[requestId].type));

    let sourcePath = '/data/diag.cfg';
    let directoryPath = this._getDestDicPath(this._requestInfo[requestId].storeToDevice,
       this._requestInfo[requestId].type);
    let fileName = null;

    cpmm.sendAsyncMessage('JrdSrv:CopyFile',
      {sourcePath: sourcePath, directoryPath: directoryPath, fileNewName: fileName, requestID: requestId});
  },

  _runProcess: function(requestId) {
    let type = this._requestInfo[requestId].type;
    let aStoreToDevice = this._requestInfo[requestId].storeToDevice;

    debug('_runProcess: ' + JSON.stringify(type) +
      '  store_to_device: ' + JSON.stringify(aStoreToDevice));

    let startRunStr = 'start ' + this._getRunStr(type);;
    let dest_str = this._getDestWholePath(aStoreToDevice, type);

    if( 'qxdm' === type) {
      startRunStr += (' -o ' + dest_str);
    }
    else {
      startRunStr += (' >' + dest_str);
    }
    cpmm.sendAsyncMessage('JrdSrv:RunProcess', {command: startRunStr, requestID: requestId});
  },

   //tcl_congqingbin "command type check for file creation"
   _isLegalPath: function(aProcessType){
     if(aProcessType === 'logcat' || aProcessType === 'dmesg' || aProcessType === 'qxdm' ||
         aProcessType === 'bugreport'  || aProcessType === 'tombstones'){
         return true;
     }else{
         return false;
     }

  },

  //--DOM API--: run()   
  run: function(aParam, aStoreToDevice) {
    debug('run: ' + ' aParam: ' + aParam + ' aStoreToDevice: ' + aStoreToDevice);
    let request = this.createRequest();
    let request_id = this.getRequestId(request);
    

    
    if(this._isLegalPath(aParam)){   //tcl_congqingbin "security check for file creation"                                                
        let path = this._getDestDicPath(aStoreToDevice, aParam);

        cpmm.sendAsyncMessage('JrdSrv:CreateDirOrFile',
            {path: path, typeStr: 'DIRECTORY', shouldCreate: true, requestID: request_id});
        this._requestInfo[request_id] = {};
        this._requestInfo[request_id].type = aParam;
        this._requestInfo[request_id].storeToDevice = aStoreToDevice;

    }else{
        let data = {};
        data.type = aParam;
        data['errorInfo'] = 'Store log to a wrong place';
        data['errorType'] = 'IllegalFilePathToStoreLog';
        debug('IllegalFilePathToStoreLog' + JSON.stringify(data));
        Services.DOMRequest.fireErrorAsync(request, JSON.stringify(data));
    }
    return request;
  },

  //--DOM API--: stop()
  stop: function(aParam) {
    debug('stop: ' + JSON.stringify(aParam));
    let request = this.createRequest();
    let request_id = this.getRequestId(request);
    let runStr = this._getRunStr(aParam);

    let stopRunStr = 'stop ' + runStr;
    //diag_mdlog is one specail case. We should run 'diag_mdlog -k' after run 'diag_mdlog'
    //Otherwise hs will not create the diag_log file.
    if (aParam === 'qxdm') {
      stopRunStr = 'start ' + '/system/bin/diag_mdlog -k';
    }
    cpmm.sendAsyncMessage('JrdSrv:RunProcess', {command: stopRunStr, requestID: request_id});

    this._requestInfo[request_id] = {};
    this._requestInfo[request_id].type = aParam;

    return request;
  },

/*----jrd_tyin add for mmitest audio_loop start----*/
  //--DOMAPI--: startAudioLoopTest()
  // aParam is 'mic' or 'headset-mic'
  startAudioLoopTest: function(aParam) {
    debug('mmitest--startAudioLoopTest: ' + aParam);
    cpmm.sendAsyncMessage('JrdSrv:AudioLoop', {param: aParam, operation: 'start'});
  },

  stopAudioLoopTest: function() {
    debug('mmitest--stopAudioLoopTest.');
    cpmm.sendAsyncMessage('JrdSrv:AudioLoop', {param: 'mic', operation: 'stop'}); // for stop, do not care param.
  },
/*----jrd_tyin add for mmitest audio_loop end----*/

/*----jrd_tyin add for mmitest force in call start.----*/
  startForceInCall: function() {
    cpmm.sendAsyncMessage('JrdSrv:ForceInCall', {operation: 'start'});
  },
  stopForceInCall: function() {
    cpmm.sendAsyncMessage('JrdSrv:ForceInCall', {operation: 'stop'});
  },
/*----jrd_tyin add for mmitest force in call end.----*/

/*----jrd_tyin add for UniversalCommand start----*/
  //note: if not use shell, isUseShell = 0, "command" should be one command.
  startUniversalCommand: function(command, isUseShell) {

    let request = this.createRequest();

    cpmm.sendAsyncMessage('JrdSrv:UniversalCommand', {param: command, useShell: isUseShell, operation: 'start', requestID: this.getRequestId(request)});

    return request;
  },

  /*If the command is endless, then need stop it manually*/
  stopUniversalCommand: function() {
    cpmm.sendAsyncMessage('JrdSrv:UniversalCommand', {operation: 'stop'});
  },
/*----jrd_tyin add for UniversalCommand end----*/

//jrd_tyin add, minitest->gps_test start
//minitest->gps_test need enable at the beginning of minitest for speed up.
  startGpsTest: function() {
    cpmm.sendAsyncMessage('JrdSrv:GpsTest', {operation: 'start'});
  },

  stopGpsTest: function() {
    cpmm.sendAsyncMessage('JrdSrv:GpsTest', {operation: 'stop'});
  },
//jrd_tyin add, minitest->gps_test end


/*--- dingp@tcl.com Add for Charger LED Test ---*/
  setChargerLed: function(config) {
    debug('SetChargerLed');
    cpmm.sendAsyncMessage('JrdSrv:SetChargerLED', {config: config});
  },

/*--- dingp@tcl.com Add for Camera LED Test ---*/
  setCameraLed: function(config) {
    debug('SetCameraLed');
    cpmm.sendAsyncMessage('JrdSrv:SetCameraLED', {config: config});
  },


/*----jrd_tanya add for get NV data start----*/
  readNvitem: function(item) {
   debug('JrdExtension readNvitem');
   let request = this.createRequest();
   cpmm.sendAsyncMessage('JrdSrv:ReadNvitem', {item: item, requestID: this.getRequestId(request)});
   return request;
  },
  writeNvitem: function(item, length, value) {
   debug('JrdExtension writeNvitem');
   let request = this.createRequest();
   cpmm.sendAsyncMessage('JrdSrv:WriteNvitem', {item: item, length: length, value: value, requestID: this.getRequestId(request)});
   return request;
  },
/*----jrd_tanya add for get NV data end----*/
  //--DOM API--: getFilesLastTime()
  getFilesLastTime: function(aParamAarry, aLength) {
    debug('getLastJrdLogFileTime: ' + aParamAarry.toString());
    let request = this.createRequest();
    let aFilePathArray = [];

    for (let i = 0; i < aParamAarry.length; i++) {
      aFilePathArray[aFilePathArray.length] =
        this._getDestDicPath('PHONE', aParamAarry[i]) + aParamAarry[i] + '*';
      aFilePathArray[aFilePathArray.length] =
        this._getDestDicPath('SDCARD', aParamAarry[i]) + aParamAarry[i] + '*';
    }

    cpmm.sendAsyncMessage('JrdSrv:GetFilesLastTime', {aFilePathArray: aFilePathArray, requestID: this.getRequestId(request)});
    return request;
  },

  //DOM API to check file is exist or not.
  checkIsFileExist: function(filePath) {
    debug('checkIsFileExist: ' + ' filePath: ' + filePath);
    let request = this.createRequest();
    let request_id = this.getRequestId(request);

    cpmm.sendAsyncMessage('JrdSrv:CreateDirOrFile',
        {path: filePath, typeStr: 'FILE', shouldCreate: false, requestID: request_id});

    this._requestInfo[request_id] = {};
    this._requestInfo[request_id].type = 'check';

    return request;
  },

  //DOM API to check command is running or not.
  checkIsCommandRunning: function(aParam) {
    debug('checkIsCommandRunning: ' + ' aParam: ' + aParam);
    let pattern = /(\S+)\s+([^>]*)(?:>\s*(\S+))?/;
    let result;

    let runStr = this._getRunStr(aParam);

    if (result = pattern.exec(runStr)) {
      let program = result[1];
      let request = this.createRequest();
      let request_id = this.getRequestId(request);
      cpmm.sendAsyncMessage('JrdSrv:CheckIsCommandRunnig',
          {commands: program, requestID: request_id});
      return request;
    }

    return null;
  },

  //--DOM API--: attribute jsval onkeyevent
  set onkeyevent(keyEvtHandle) {
    debug('onkeyevent');
    if (this._onKeyEvent === keyEvtHandle) {
      debug('onkeyevent: redundant setting...');
    }
    else if (this._onKeyEvent && keyEvtHandle) {
      debug('onkeyevent: key handler changed');
      //We don't support event processing chain for the moment. Just replace it
      this._onKeyEvent = keyEvtHandler;
    }
    else if (this._onKeyEvent === null) {
      debug('onkeyevent: key handler set');
      this._onKeyEvent = keyEvtHandle;
      cpmm.sendAsyncMessage('JrdSrv:HookKeyEvt', {enable: true});
    }
    else {
      debug('onkeyevent: key handler unset');
      this._onKeyEvent = null;
      cpmm.sendAsyncMessage('JrdSrv:HookKeyEvt', {enable: false});
    }
  },

  //--DOM API--: add headset start
  set onHeadsetStatusChanged(headsetStatusChangedEvtHandle) {
    if (headsetStatusChangedEvtHandle) {
      this._onHeadsetStatusChanged = headsetStatusChangedEvtHandle;
      cpmm.sendAsyncMessage('JrdSrv:HookHeadsetStatusChangedEvt', {enable: true});
    }
    else {
      this._onHeadsetStatusChanged = null;
      cpmm.sendAsyncMessage('JrdSrv:HookHeadsetStatusChangedEvt', {enable: false});
    }
  },

  //JRD_KLWANG add for CR:414530---begin
  readRoValue: function(ro) {
    let svn = null;

    svn = libcutils.property_get(ro);
    debug("svn: " + svn);
    return svn;
  },
  //JRD_KLWANG add for CR:414530---end

  receiveMessage: function(aMessage) {
    debug('receiveMessage: ' + JSON.stringify(aMessage));
    let request;
    let msg = aMessage.data;
    switch (aMessage.name) {
      case 'JrdSrv:Common:Return':
        request = this.takeRequest(msg.requestID);
        if (request) {
          if (msg.data.result === 'OK') {
            debug('Success: ' + JSON.stringify(msg));
            Services.DOMRequest.fireSuccess(request, exposeReadOnly(msg.data.data));
          }
          else if (msg.data.result === 'KO') {
            debug('Fail: ' + JSON.stringify(msg));
            let errorInfo;
            if (typeof msg.data.data === 'object') {
              errorInfo = JSON.stringify(msg.data.data);
            }
            else {
              errorInfo = msg.data;
            }
            debug('Send data:' + errorInfo);
            Services.DOMRequest.fireError(request, errorInfo);
          }
        }
        else {
          debug('Success: Can not find the request object with ' + msg.requestID);
        }
        break;

      case 'JrdSrv:RunProcess:Return':
      case 'JrdSrv:CopyFile:Return':
        this._processMsgHdlr(aMessage);
        break;

      case 'JrdSrv:GetFilesLastTime:Return':
        this._filesLastTimeMsgHdlr(msg);
        break;

      case 'JrdSrv:CreateDirOrFile:Return':
        this._createFileMsgHdlr(msg);
        break;

      case 'JrdSrv:CheckIsCommandRunnig:Return':
        let request = this.takeRequest(msg.requestID);
        Services.DOMRequest.fireSuccess(request, exposeReadOnly(msg.status));
        break;

      case 'JrdSrv:KeyEvent:Return':
        if (this._onKeyEvent !== null) {
          debug('To process Key event' + JSON.stringify(msg));

          let event = new this._window.Event(msg.event);
          this._onKeyEvent.handleEvent(event);
        }
        else {
          debug('The key processor is unregistered');
        }
        break;

      case 'JrdSrv:HeadsetStatusChangedEvt:Return':
        if (this._onHeadsetStatusChanged !== null) {
          let event = new this._window.Event(msg.event);
          this._onHeadsetStatusChanged.handleEvent(event);
        }
        break;

//jrd_tyin add for UniversalCommand start.
      case 'JrdSrv:UniversalCommand:Return':
        request = this.takeRequest(msg.requestID);
        if (request) {
          if (msg.data === 'process-finished') {
            Services.DOMRequest.fireSuccess(request, exposeReadOnly(msg.data));
          }
          else {
            Services.DOMRequest.fireError(request, exposeReadOnly(msg.data));
          }
        }
        else {
          debug('Success: Can not find the request object with ' + msg.requestID);
        }
        break;
//jrd_tyin add for UniversalCommand end.
//jrd_tanya add for read nvram return start.
      case 'JrdSrv:ReadNvitem:Return':
        debug('tanya:'+ msg.requestID);
        request = this.takeRequest(msg.requestID);
        if (request) {
          if (msg.data.result === 'OK') {
            debug('Success: ' + JSON.stringify(msg));
            Services.DOMRequest.fireSuccess(request, exposeReadOnly(msg.data.data));
          }
          else if (msg.data.result === 'KO') {
            debug('Fail: ' + JSON.stringify(msg));
            Services.DOMRequest.fireError(request, msg.data.data);
          }
        }
        else {
          debug('Success: Can not find the request object with ' + msg.requestID);
        }
  	break;

      case 'JrdSrv:WriteNvitem:Return':
        debug('jrdextension.js case JrdSrv:WriteNvitem:Return' + msg.requestID);
        request = this.takeRequest(msg.requestID);
        // dingp@tcl.com Modified for NVram access at 2013-12-31 01:28:53 PM
        //Services.DOMRequest.fireSuccess(request, exposeReadOnly());
        Services.DOMRequest.fireSuccess(request, exposeWrite(msg.data.data));

  	break;
//jrd_tanya add for read nvram return start.
      default:
        debug('Error Message' + aMessage.name);
        break;
    }
  },

  // dingp@tcl.com add for hide factory mode dial code at 2014-02-24 11:19:51 AM
  getSysCode: function(code) {

    let a = '';

    debug('sysInfoRead ============= code = ' + code);
    if ('*#3228#' === code) {
      a = '1001';
    } else if ('*#2886#' === code) {
      a = '1002';
    } else if ('*#573564#' === code) {
      a = '1003';
    } else if ('*#1201195#' === code) {
      a = '1004';
    } else if ('*#1201194#' === code) {
      a = '1005';

    } else if ('*#06#' === code) {
      a = '2001';
    } else if ('*#837837#' === code) {
      a = '2002';
    } else if ('###232#' === code) {
      a = '2003';

    } else {
      a = '1000'
    }
    return a;
  },


	fileRead: function(file) {
		dump('[WCL] | [Jrd] | file: ' + file + '\n');
		let a = this._fileRead(file);
		return a;
	}
};

var components = [jrdIDOMExtension];
if ('generateNSGetFactory' in XPCOMUtils)
  this.NSGetFactory =
    XPCOMUtils.generateNSGetFactory(components);  // Firefox 4.0 and higher
else
  this.NSGetModule =
    XPCOMUtils.generateNSGetModule(components);    // Firefox 3.x

