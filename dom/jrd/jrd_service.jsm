//JRD_ZMTANG create on 2012-09-04

'use strict';

var DEBUG = 1;
if (DEBUG)
  var debug = function(s) { dump('<TZM_LOG> -*- jrd_service.jsm -*-: ' + s + '\n'); };

else
  var debug = function(s) {};

const Cu = Components.utils;
const Cc = Components.classes;
const Ci = Components.interfaces;
const Cr = Components.results;

this.EXPORTED_SYMBOLS = ['JrdService'];

Cu.import('resource://gre/modules/XPCOMUtils.jsm');
Cu.import('resource://gre/modules/Services.jsm');
Cu.import('resource://gre/modules/FileUtils.jsm');
Cu.import('resource://gre/modules/NetUtil.jsm');

var HEADSET_KEY = 'Insert detect insert = ';

//JRD_KLWANG add for CR:414530---begin
XPCOMUtils.defineLazyGetter(this, "libcutils", function () {
  Cu.import("resource://gre/modules/systemlibs.js");
  return libcutils;
});
//JRD_KLWANG add for CR:414530---end

XPCOMUtils.defineLazyServiceGetter(this, 'ppmm',
                                   '@mozilla.org/parentprocessmessagemanager;1',
                                   'nsIMessageListenerManager');

/*----jrd_tyin add for mmitest force in call start.----*/
const nsIAudioManager = Ci.nsIAudioManager;

XPCOMUtils.defineLazyGetter(this, "gAudioManager", function getAudioManager() {
  try {
    return Cc["@mozilla.org/telephony/audiomanager;1"]
             .getService(nsIAudioManager);
  } catch (ex) {
    return null;
  }
});
/*----jrd_tyin add for mmitest force in call end.----*/


this.JrdService = {
  init: function() {
    debug('Initialize');

    this._messages = ['JrdSrv:Common',
      'JrdSrv:HookHeadsetStatusChangedEvt',
      'JrdSrv:HookKeyEvt',
      'JrdSrv:RunProcess',
      'JrdSrv:GetFilesLastTime',
      'JrdSrv:CopyFile',
      'JrdSrv:CreateDirOrFile',
      'JrdSrv:AudioLoop', //jrd_tyin add for mmitest audio_loop
      'JrdSrv:ForceInCall', //jrd_tyin add for mmitest force in call.
      'JrdSrv:UniversalCommand', //jrd_tyin add for UniversalCommand
      'JrdSrv:GpsTest', //jrd_tyin add for mmitest->gps_test
      'JrdSrv:CheckIsCommandRunnig',
      'JrdSrv:ReadNvitem', //jrd_tanya for ReadNvitem
      'JrdSrv:WriteNvitem', //jrd_tanya for WriteNvitem
      'JrdSrv:SetChargerLED', //jrd_tanya for Set Charger LED
      'JrdSrv:SetCameraLED' //jrd_tanya for Set Camera LED
      ];
    this._messages.forEach((function(msgName) {
        ppmm.addMessageListener(msgName, this);
      }).bind(this));

    this._keyEvtTarget = null;

    this._headsetEvtTarget = null;

    this._audioLoopProcess = null; //jrd_tyin add for mmitest audio_loop

    this._gpsTestProcess = null; //jrd_tyin add for mmitest->gps_test

    this._universalCommandProcess = null;//jrd_tyin add for UniversalCommand
    //JRD_KLWANG add for CR:414530.Get svn content and write it into "NV_UE_IMEISV_SVN_I"---begin
#ifdef MOZ_WIDGET_GONK
    let svn = null;
    svn = libcutils.property_get('ro.def.software.svn');
    debug("svn: " + svn);
    if(null != svn && svn.length >= 2) {
      // Currently, we must use "128", because "JrdNvAccess::WriteNvitem" interface will check the count with sizeof(nv_item_type)
      let array = new Array(128);
            
      //The max length of svn is 2
      array[0] = Number(svn.charAt(0))*16+Number(svn.charAt(1));
      // dingp@tcl.com NVram access changed remove this. 2013-12-31 11:36:49 AM
      // NV_UE_IMEISV_SVN_I = 5153(nv_items.h)
      /*
      let obj = this._readNvitem(5153, null);
      if(obj.result === 'OK') {
        if(array[0] != obj.data[0]) {
          this._writeNvitem(5153, 128, array, null);
        }
      }
      else {
        this._writeNvitem(5153, 128, array, null);
      }*/
     }
    //JRD_KLWANG add for CR:414530---end
#endif
  },

  get isKeyHooked() {
    //FIXME: What if the CP is termiated without un-hooking the key event ?
    debug('isKeyHooked = ' + (this._keyEvtTarget !== null));
    return (this._keyEvtTarget !== null);
  },

  processKeyEvt: function(evt) {
    debug('processKeyEvt');
    if (this._keyEvtTarget !== null) {
      debug('processKeyEvt: Send key event ' + JSON.stringify(evt));
      this._keyEvtTarget.sendAsyncMessage('JrdSrv:KeyEvent:Return', {event: evt});
    }
    else {
      debug('processKeyEvt: No responsibler for key evt processing');
    }
  },

  get isHeadsetStatusChangedHooked() {
    debug('isHeadsetStatusChangedHooked');
    return (this._headsetEvtTarget !== null);
  },

  processHeadsetStatusChangedEvt: function(evt) {
    debug('processHeadsetStatusChangedEvt');
    if (this._headsetEvtTarget !== null) {
      this._headsetEvtTarget.sendAsyncMessage('JrdSrv:HeadsetStatusChangedEvt:Return', {event: evt});
    }
  },

  //To get the given file or directory 's status,if the parameter isCreate is false,we just check
  //whether the given file or directory is exist,otherwise we'll create it if it not exist.
  _createGivenDirOrFile: function(path, typeStr, shouldCreate, callback) {
    debug('_createGivenDirOrFile--path:' + path);

    let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
    file.initWithPath(path);
    if (file.exists()) { //Directory or file is exist
      callback('EXIST');
    }
    else if (shouldCreate) { //Directory or file is not exist, and create this directory or file
      let temp = 'CREATE_SUCCESS';
      try {
        if (typeStr === 'DIRECTORY') { //Directory case
          file.create(Ci.nsIFile.DIRECTORY_TYPE, FileUtils.PERMS_DIRECTORY);
        }
        else { //File case
          file.create(Ci.nsIFile.NORMAL_FILE_TYPE, FileUtils.PERMS_FILE);
        }
      }
      catch (e) {
        debug('create file fail: ' + e);
        temp = 'CREATE_FAILED';
      }
      finally {
        callback(temp);
      }
    }
    else { //Directory or file is not exist, and callback directly, don't create it.
      callback('NO_EXIST');
    }
  },

  //To check is the given command running.The result is true or false.
  _checkIsCommandRunning: function(commands, callback) {
    debug('_checkIsCommandRunning' + ' commands: ' + commands);
    let is_running = false;
    if (commands in this._sysRunningSrv) {
      is_running = true;
    }
    callback(is_running);
  },

  //Compare str1 with str2. Notes: str1 can contain '*' at the before or end.
  _strCompare: function(str1, str2) {
    let prop = /[^\*]\S+[^\*]/;
    let ret = prop.exec(str1);
    let tempStr = ret[0];
    let result = false;

    if (str1 && str2) {
      if (str1[0] === '*' && str1[str1.length - 1] === '*') {
        if (str2.indexof(tempStr) !== -1) {
          result = true;
        }
      }
      else if (str1[str1.length - 1] === '*') {
        if (str2.length >= tempStr.length &&
          tempStr === str2.substr(0, tempStr.length)) {
          result = true;
        }
      }
      else if (str1[0] === '*') {
        if (str2.length >= tempStr.length &&
          tempStr === str2.substr(str2.length - tempStr.length, tempStr.length)) {
          result = true;
        }
      }
      else {
        if (tempStr === str2) {
          result = true;
        }
      }
    }
    return result;
  },

 _readSysFile: function(filePath, callback) {
    let file = Cc['@mozilla.org/file/local;1'].createInstance(Ci.nsILocalFile);
    debug('_readSysFile entry.');
    file.initWithPath(filePath);
    NetUtil.asyncFetch(file, function(inputStream, status) {
        let temp = {};
        if (!Components.isSuccessCode(status)) {//Error
          debug('_readSysFile:failed');
          temp.result = 'KO';
          temp.data = status;
        }
        else {//Success
          let data = NetUtil.readInputStreamToString(
            inputStream, inputStream.available());
          debug('_readSysFile:success');
          temp.result = 'OK';
          temp.data = data;
        }
        callback(temp);
    });
  },

  _sysRunningSrv: {},

  _sysService: function(command, callback) {
    debug('sysService: ' + command);

    //Example: start /bin/logcat -b main >/sdcard/jrdlog/logcat_20120925_1108.txt
    let pattern = /(start|stop)\s+(\S+)\s+([^>]*)(?:>\s*(\S+))?/;
    let result;
    let cbData = {};
	
    if (result = pattern.exec(command)) {
      let start = (result[1] === 'start') ? true : false;
      let program = result[2];
      let argument = result[3];
      let output = result[4];

      if (start === true) {
        //diag_mdlog is one specail case. We should run 'diag_mdlog -k' after run 'diag_mdlog'
        //Otherwise hs will not create the diag_log file.
        if (program in this._sysRunningSrv &&
          program !== '/system/bin/diag_mdlog') {
          debug('The servcie ' + program + ' is still running');
          return;
        }

        let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
        let process = Components.classes['@mozilla.org/process/util;1'].createInstance(Components.interfaces.nsIProcess);
        let args = [];

        if (output) {
          debug('program, Arguments and output: ' + program + ' ' + argument + ' >' + output);

          //file.initWithPath('/bin/bash');
          file.initWithPath('/system/bin/sh');
          args.push('-c', program + ' ' + argument + ' >' + output);
          debug('args' + args);
        }
        else {
          debug('program, Arguments: ' + program + ' ' + argument);
          file.initWithPath(program);
          args = argument.trim().split(/\s+/);
        }

        try {
          process.init(file);

          let that = this;
          process.runAsync(args, args.length, {
            observe: function(aSubject, aTopic, aData) {
              debug('observer: ' + aSubject + ':' +
                aTopic + ':' + aData + ':' + program);

              for (let name in that._sysRunningSrv) {
                if (that._sysRunningSrv[name] === process) {
                  debug('!!!Remove ' + name + ' from running service');
                  delete that._sysRunningSrv[name];
                  break;
                }
              }
              cbData['processState'] = aTopic;
              callback(cbData);
            }
          });
          this._sysRunningSrv[program] = process;
          cbData['processState'] = 'process-running';
          callback(cbData);
        }
        catch (e) {
          cbData['processState'] = 'process-failed';
          cbData['errorType'] = 'DefaultError';
          callback(cbData);
        }
      }
      else {
        if (program in this._sysRunningSrv) {
          debug('Stopping service ' + program);
          //Please refer the observe function, callback will be runned after the process is killed.
          //But message can't be sent successfully  because the process has been killed
          //So we should callback before kill process.
          cbData['processState'] = 'process-finished';
          callback(cbData);
          this._sysRunningSrv[program].kill();
        }
      }
    }
  },

  _copyFile: function(sourcePath, destPath, fileNewName, callback) {
    debug('copyFile: ' + sourcePath + ' destPath: ' + destPath + ' fileName:' + fileNewName);

    let sourcefile = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
    sourcefile.initWithPath(sourcePath);

    let destfile = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
    destfile.initWithPath(destPath);

    let cbData = {};
    cbData['processState'] = 'process-finished';
    try {
      sourcefile.copyTo(destfile, fileNewName);
    }
    catch (e) {
      debug("error: " + e.result);
      cbData['processState'] = 'process-failed';
      if(e.result == Cr.NS_ERROR_FILE_TARGET_DOES_NOT_EXIST)
      {
        cbData['errorType'] = 'SourceFileNotExist';
      }
      else
      {
        cbData['errorType'] = 'DefaultError';
      }
    }
    finally {
      callback(cbData);
    }
  },

  _getFilesLastTime: function(aFilePathArray, callback) {
    debug('_getFilesLastTime   aFilePathArray: ' + aFilePathArray.toString());

    let pattern = /(\/(\S+)\/)([^\/]+)/;
    let i = 0;
    let obj = {}; //we will return this obj to the users
    let pathObj = {};

    //filePath: /mnt/sdcard/jrdlog/bugreport*
    //pathObj: {'/mnt/sdcard/jrdlog/':['bugreport*', 'logcat*'], '/data/':['bugreport*', 'logcat*']}
    for (i = 0; i < aFilePathArray.length; i++) {
      let result = pattern.exec(aFilePathArray[i]);
      if (result) {
        if (!(result[1] in pathObj)) {
          pathObj[result[1]] = [];
        }
        pathObj[result[1]].push(result[3]);

        let prop = /[^\*]\S+[^\*]/;
        let ret = prop.exec(result[3]);
        obj[ret[0]] = {time: 0};
      }
    }
    debug(JSON.stringify(obj));
    debug(JSON.stringify(pathObj));
    for (let directoryPath in pathObj) {
      let directory = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
      directory.initWithPath(directoryPath);

      if (directory.exists()) {
        let fileList = directory.directoryEntries;
        let fileNameAarry = pathObj[directoryPath];

        while (fileList.hasMoreElements()) {
          let file = fileList.getNext().QueryInterface(Components.interfaces.nsILocalFile);
          for (i = 0; i < fileNameAarry.length; i++) {
           if (this._strCompare(fileNameAarry[i], file.leafName)) {
             let prop = /[^\*]\S+[^\*]/;
             let ret = prop.exec(fileNameAarry[i]);
             if (obj[ret[0]].time < file.lastModifiedTime) {
               obj[ret[0]].time = file.lastModifiedTime;
               obj[ret[0]].location = directoryPath;
             }
           }
          }
        }
      }
    }
    callback(obj);
  },

   //jrd_tanya add for readNvitem start.
   _readNvitem: function(item,callback){
    debug("jrd_service.jsm _readNvitem: enter");
    let obj = {};
    let jrdNvAccess = Components.classes['@jrdcom.com/JrdNvAccess;1'].createInstance(Components.interfaces.nsIJrdNvAccess);
    try {
      obj.data = jrdNvAccess.readNvitem(item);
      obj.result = 'OK';
      debug("jrd_service.jsm _readNvitem: size: " + obj.data.length +  ", value: " + obj.data.toString());
    }
    catch(e) {
      obj.result = 'KO';
      debug("read nvram fail: " + e.result);
      obj.data = e.result; //Maybe can provide better info to gaia
    }
    if(callback !== null) {
      callback(obj);
    }
    return obj;
  },
  //jrd_tanya add for readNvitem end.

   //jrd_tanya add for writeNvitem start.
   _writeNvitem: function(item, length, value, callback){
    let jrdNvAccess = Components.classes['@jrdcom.com/JrdNvAccess;1'].createInstance(Components.interfaces.nsIJrdNvAccess);
    debug("jrdnvram write item = " + item + "value = " + value );
    jrdNvAccess.writeNvitem(item, length, value);
    debug("jrdnvram: write finished");
    if(callback !== null) {
      callback();
    }
  },
  //jrd_tanya add for writeNvitem end.

  // dingp@tcl.com add for Charger LED test
  _setchargerled: function(config) {

    try {
      let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
      file.initWithPath("/sys/class/leds/charger_light/brightness");
      var foStream = Components.classes["@mozilla.org/network/file-output-stream;1"].createInstance(Components.interfaces.nsIFileOutputStream);
      foStream.init(file, 0x02 | 0x08 | 0x20, "0666", 0);
      var converter = Components.classes["@mozilla.org/intl/converter-output-stream;1"].createInstance(Components.interfaces.nsIConverterOutputStream);
      converter.init(foStream, "UTF-8", 0, 0);
      converter.writeString(config);
      converter.close();
    } catch (ex) {
      debug('setchargerled io error! ex = ' + ex);
    }
  },

  // dingp@tcl.com add for CameraLED test
  _setcameraled: function(config) {

    try {
      let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
      file.initWithPath("/sys/class/leds/flashlight/brightness");
      var foStream = Components.classes["@mozilla.org/network/file-output-stream;1"].createInstance(Components.interfaces.nsIFileOutputStream);
      foStream.init(file, 0x02 | 0x08 | 0x20, "0666", 0);
      var converter = Components.classes["@mozilla.org/intl/converter-output-stream;1"].createInstance(Components.interfaces.nsIConverterOutputStream);
      converter.init(foStream, "UTF-8", 0, 0);
      converter.writeString(config);
      converter.close();
    } catch (ex) {
      debug('setcaeraled io error! ex = ' + ex);
    }
  },



  receiveMessage: function(aMessage) {
    debug('receiveMessage: ' + aMessage.name);
    let mm = aMessage.target;
    let msg = aMessage.data;

    switch (aMessage.name) {
      case 'JrdSrv:Common':

        switch (msg.type) {
          case 'MAC_ADDR':
            this._readSysFile('/system/etc/wifi/macaddr',
              function(data) {
                mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
              });
            break;

          case 'BATTERY_TEMP':
            this._readSysFile('/sys/class/power_supply/battery/temp',
              function(data) {
                mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
              });
            break;
          
          case 'USB_CONNECT':
            this._readSysFile('/sys/devices/virtual/android_usb/android0/state',
              //For usb is different from the other requst,it needs to return true or false.
              function(data) {
                if (data.result === 'OK') {
                  let s = data.data.match('CONFIGURED');
                  if (s == 'CONFIGURED') {
                    data.data = true;
                  }
                  else {
                    data.data = false;
                  }
                  mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
                }
                else {
                  mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
                }
              });
              break;

          case 'HEADPHONE_STATE':
            this._readSysFile('/sys/kernel/debug/wcd9xxx_mbhc',
              //For headphone is different from the other requst,it needs to return on or off.
              function(data) {
                if (data.result === 'OK') {
                  var index = data.data.indexOf(HEADSET_KEY);
                  var result = data.data.substring(index + HEADSET_KEY.length);
                  if (result == 1) {
                    data.data = 'on';
                  }
                  else {
                    data.data = 'off';
                  }
                  mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
                }
                else {
                  mm.sendAsyncMessage('JrdSrv:Common:Return', {data: data, requestID: msg.requestID});
                }
              });
              break;

          default:
            debug('receiveMessage: <Jrd:Common> can not find type ' +
              JSON.stringify(msg.type));
            break;
        }
        break;

      case 'JrdSrv:HookKeyEvt':
        if (msg.enable === true) {
          debug('enabling key hook');
          //FIXME: What if the CP is terminated at some time?
          this._keyEvtTarget = mm;
        }
        else {
          debug('disabling key hook');
          this._keyEvtTarget = null;
        }
        break;
      case 'JrdSrv:HookHeadsetStatusChangedEvt':
        if (msg.enable === true) {
          this._headsetEvtTarget = mm;
        }
        else {
          this._headsetEvtTarget = null;
        }
        break;
      case 'JrdSrv:RunProcess':
        this._sysService(msg.command,
          function(processState) {
              mm.sendAsyncMessage('JrdSrv:RunProcess:Return', {data: processState, requestID: msg.requestID});
          });
        break;

      case 'JrdSrv:GetFilesLastTime':
        this._getFilesLastTime(msg.aFilePathArray,
          function(data) {
              mm.sendAsyncMessage('JrdSrv:GetFilesLastTime:Return', {data: data, requestID: msg.requestID});
          });
        break;

      case 'JrdSrv:CopyFile':
        this._copyFile(msg.sourcePath,
          msg.directoryPath,
          msg.fileNewName,
          function(data) {
            mm.sendAsyncMessage('JrdSrv:CopyFile:Return', {data: data, requestID: msg.requestID});
          });
        break;

      case 'JrdSrv:CreateDirOrFile':
        this._createGivenDirOrFile(msg.path,
          msg.typeStr,
          msg.shouldCreate,
          function(data) {
              mm.sendAsyncMessage('JrdSrv:CreateDirOrFile:Return', {data: data, requestID: msg.requestID});
          });
        break;

      case 'JrdSrv:CheckIsCommandRunnig':
        this._checkIsCommandRunning(msg.commands, function(is_running) {
          mm.sendAsyncMessage('JrdSrv:CheckIsCommandRunnig:Return', {status: is_running, requestID: msg.requestID});
        });
        break;

      //jrd_tyin add for mmitest audio_loop start
      case 'JrdSrv:AudioLoop':
        debug(' msg.param ======================== ' + msg.param);
        if(msg.operation === 'start') {
          let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
          let process = Components.classes['@mozilla.org/process/util;1'].createInstance(Components.interfaces.nsIProcess);
          let args = [];
          
          // dingp@tcl.com modified for Audio elements test --start
          file.initWithPath('/system/bin/mm-audio-ftm');
          //Request: volume too loud. change the volume for both mic(4 -> 2 and headset-mic 4->1)
          // dingp@tcl.com Modified test command for audio test at 2014-01-03
          if(msg.param === 'mic') {
            args = ['-tc', '19', '-c', '/etc/ftm_test_config', '-d', '8', '-v', '88'];
          }
          else if(msg.param === 'sub-mic') {
            //args = ['-tc', '19', '-c', '/etc/ftm_test_config', '-d', '20', '-v', '88'];
            break;
          }
          else if(msg.param === 'headset-left') {
            args = ['-tc', '7', '-c', '/etc/ftm_test_config', '-d', '6', '-v', '45'];
          }
          else if(msg.param === 'headset-right') {
            args = ['-tc', '6', '-c', '/etc/ftm_test_config', '-d', '6', '-v', '45'];
          }
          else if(msg.param === 'headset-mic') {
            args = ['-tc', '22', '-c', '/etc/ftm_test_config', '-d', '8', '-v', '88'];
          }
          else {
            //args = ['-tc', '22', '-c', '/etc/ftm_test_config', '-d', '60', '-v', '88'];
            break;
          }

          try {
            let self = this;
            process.init(file);
            debug('mmitest--audio loop: init finished');
            process.runAsync(args, args.length, {
              observe: function() {
                if (self._audioLoopProcess && self._audioLoopProcess.isRunning) {
                  self._audioLoopProcess.kill();
                  self._audioLoopProcess = null;
                  debug('mmitest--KILL by observe******');
                }
              }
            });
            debug('           mmitest: _audioLoopProcess: ' + this._audioLoopProcess);
            this._audioLoopProcess = process;
          }
          catch (e) {
            debug('mmitest--audio loop: start process error:' + e);
          }
        }
        else if(msg.operation === 'stop') {
          debug('mmitest--audio loop: stop process');
          if(this._audioLoopProcess && this._audioLoopProcess.isRunning) {
            debug('             mmitest: audio loop: do KILL******');
            this._audioLoopProcess.kill();
          }
          this._audioLoopProcess = null;
        }
        break;
      //jrd_tyin add for mmitest audio_loop end

      //jrd_tyin add for mmitest force in call start.
      case 'JrdSrv:ForceInCall': 
        if(msg.operation === 'start') {
          if(gAudioManager) {
            gAudioManager.phoneState = nsIAudioManager.PHONE_STATE_IN_CALL;
            debug('mmitest--startForceInCall: set state finished');
          }
        }
        else if(msg.operation === 'stop') {
          if(gAudioManager) {
            gAudioManager.phoneState = nsIAudioManager.PHONE_STATE_NORMAL;
            debug('mmitest--stopForceInCall: reset state finished');
          }
        }
        break;
      //jrd_tyin add for mmitest force in call end.

      //jrd_tyin add for UniversalCommand start.
      case 'JrdSrv:UniversalCommand':
        if(msg.operation === 'start') {
          let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
          let process = Components.classes['@mozilla.org/process/util;1'].createInstance(Components.interfaces.nsIProcess);
          let args = [];
          let s = null;
          let cmd = null;
          let cmds = [];

          if (msg.useShell) {
            file.initWithPath('/system/bin/sh');
            args[0] = '-c';
            args[1] = msg.param;

            debug('hwtest--args:' + args);
          }
          else {
            s = msg.param;
            cmd = s.match(/\S+/);
            file.initWithPath(cmd);

            cmds = s.match(/\S+/g);
            for (var i in cmds) {
              if (i>0) {
                args[i-1] = cmds[i];
              }
            }

            debug('hwtest--command:' + cmd + args);
          }

          try {
            let self = this;
            process.init(file);
            debug('hwtest--UniversalCommand: init finished');
            process.runAsync(args, args.length, {
              observe: function(aSubject, aTopic, aData) {
                debug('hwtest--*********observe********');
                debug('hwtest-- aSubject:' + aSubject);
                debug('hwtest-- aTopic:' + aTopic);
                debug('hwtest-- aData:' + aData);
                if (self._universalCommandProcess && self._universalCommandProcess.isRunning) {
                  self._universalCommandProcess.kill();
                  debug('hwtest--KILL by observe******');
                }
                self._universalCommandProcess = null;
                mm.sendAsyncMessage('JrdSrv:UniversalCommand:Return', {data: aTopic, requestID: msg.requestID});
              }
            });
            debug('           hwtest: _universalCommandProcess: ' + this._universalCommandProcess);
            this._universalCommandProcess = process;
          }
          catch (e) {
            debug('hwtest--universalCommand: start process error:' + e);
            mm.sendAsyncMessage('JrdSrv:UniversalCommand:Return', {data: 'process-failed', requestID: msg.requestID});
          }
        }
        else if(msg.operation === 'stop') {
          debug('hwtest--universalCommand: stop process');
          if(this._universalCommandProcess && this._universalCommandProcess.isRunning) {
            debug('             hwtest: universalCommand: do KILL******');
            this._universalCommandProcess.kill();
          }
          this._universalCommandProcess = null;
        }
        break;
      //jrd_tyin add for UniversalCommand end.

      //jrd_tyin add, minitest->gps_test start
      //minitest->gps_test need enable at the beginning of minitest for speed up.
      case 'JrdSrv:GpsTest':
        if(msg.operation === 'start') {
          let file = Components.classes['@mozilla.org/file/local;1'].createInstance(Components.interfaces.nsILocalFile);
          let process = Components.classes['@mozilla.org/process/util;1'].createInstance(Components.interfaces.nsIProcess);
          let args = [];

          file.initWithPath('/system/bin/gps_test');
          args = ['0', '1'];

          try {
            let self = this;
            process.init(file);
            debug('mmitest--gps test: init finished');
            process.runAsync(args, args.length, {
              observe: function() {
                if(self._gpsTestProcess && self._gpsTestProcess.isRunning) {
                  self._gpsTestProcess.kill();
                  self._gpsTestProcess = null;
                  debug('mmitest--KILL by observe******');
                }
              }
            });
            debug('           mmitest: _gpsTestProcess: ' + this._gpsTestProcess);
            this._gpsTestProcess = process;
          }
          catch (e) {
            debug('mmitest--gps test: start process error:' + e);
          }
        }
        else if(msg.operation === 'stop') {
          debug('mmitest--gps test: stop process');
          if(this._gpsTestProcess && this._gpsTestProcess.isRunning) {
            debug('             mmitest: gps test: do KILL******');
            this._gpsTestProcess.kill();
          }
          this._gpsTestProcess = null;
        }
        break;
      //jrd_tyin add, minitest->gps_test end
      
      //jrd_tanya add for ReadNvitem start
      case 'JrdSrv:ReadNvitem':
        this._readNvitem(
          msg.item,
          function(data){
          mm.sendAsyncMessage('JrdSrv:ReadNvitem:Return', {data: data, requestID: msg.requestID});
        });
      break;
      //jrd_tanya add for ReadNvitem end

      case 'JrdSrv:WriteNvitem':
        this._writeNvitem(
          msg.item,
	  msg.length,
	  msg.value,
          function(){
          debug("jrdnvram write finished.");
          mm.sendAsyncMessage('JrdSrv:WriteNvitem:Return', {data:'', requestID: msg.requestID});
        });
      break;

      // dingp@tcl.com add for Charger LED test
      case 'JrdSrv:SetChargerLED':
        this._setchargerled(msg.config);
        break;

      // dingp@tcl.com add for Camera LED test
      case 'JrdSrv:SetCameraLED':
        this._setcameraled(msg.config);
        break;

      default:
        debug('receiveMessage: Can not process the message ' + aMessage.name);
        break;
    }
  }

};


JrdService.init();
