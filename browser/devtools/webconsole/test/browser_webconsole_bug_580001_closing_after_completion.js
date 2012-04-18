/* vim:set ts=2 sw=2 sts=2 et: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is DevTools test code.
 *
 * The Initial Developer of the Original Code is Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  David Dahl <ddahl@mozilla.com>
 *  Patrick Walton <pcwalton@mozilla.com>
 *  Julian Viereck <jviereck@mozilla.com>
 *  Mihai Sucan <mihai.sucan@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

// Tests to ensure that errors don't appear when the console is closed while a
// completion is being performed.

const TEST_URI = "http://example.com/browser/browser/devtools/webconsole/test/test-console.html";

registerCleanupFunction(function() {
  Services.prefs.clearUserPref("devtools.gcli.enable");
});

function test() {
  Services.prefs.setBoolPref("devtools.gcli.enable", false);
  addTab(TEST_URI);
  browser.addEventListener("DOMContentLoaded", testClosingAfterCompletion,
                           false);
}

function testClosingAfterCompletion() {
  browser.removeEventListener("DOMContentLoaded",
                              testClosingAfterCompletion, false);

  openConsole();

  let inputNode = HUDService.getHudByWindow(content).jsterm.inputNode;

  let errorWhileClosing = false;
  function errorListener(evt) {
    errorWhileClosing = true;
  }

  browser.addEventListener("error", errorListener, false);

  // Focus the inputNode and perform the keycombo to close the WebConsole.
  inputNode.focus();
  EventUtils.synthesizeKey("k", { accelKey: true, shiftKey: true });

  // We can't test for errors right away, because the error occures after a
  // setTimeout(..., 0) in the WebConsole code.
  executeSoon(function() {
    browser.removeEventListener("error", errorListener, false);
    is(errorWhileClosing, false, "no error while closing the WebConsole");
    finishTest();
  });
}

