/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "nsIFileStreams.h"
#include "nsIFile.h"
#include "nsIRecoveryService.h"
#include "nsIProperties.h"
#include "nsContentUtils.h"
#include "JrdFotaNative.h"
#include "JrdINvAccess.h"


#include "nsStringGlue.h"
/*Modified by tcl_baijian 2014-03-17 the storage devices path begin*/
static char* local_session_repository_path_p = "/storage/sdcard0/downloaded";
static char* local_update_file_path_p = "/storage/sdcard0/.downloaded/update.zip";
/*Modified by tcl_baijian 2014-03-17 the storage devices path end*/
#define LOCAL_UPDAT_RESULT_FILE "/data/fota/result.txt"
#define DEVICE_PARAM_STR_MAX_LENGTH 20

/**
 * We wait 2 mins to finish for every lib. The lib will auto retry if it failed.
 * Maybe it will finish in 10s.
*/
#define TIME_INTERVAL_VALUE 1000 //millisecond
#define TIME_OUT_MAX_VALUE 120 //120 seconds

static int _selected_key_index=0;
struct cb_key
{
  int key_id;
  int key_len;
  unsigned char key_content[500];
};

#define MAX_PRIV_KEYS 10
struct cb_key _client_private_keys_info[MAX_PRIV_KEYS]=
{
   /* eCT_TCT_Android_3G_Mobile_Build_In_Fota */
   11,449, { 48,130,1,189,2,1,0,2,129,129,0,254,122,61,88,204,30,115,0,5,58,170,173,218,102,191,68,54,115,219,61,89,247,67,238,249,235,166,221,92,193,98,141,200,41,215,214,189,206,48,203,158,178,174,81,128,85,46,169,77,210,23,13,150,116,17,87,219,32,71,179,146,28,43,13,115,224,58,33,92,69,179,84,190,103,193,87,90,175,232,208,71,88,158,43,209,189,150,197,173,195,233,135,70,90,206,22,53,71,207,69,159,247,203,32,26,247,108,189,26,150,236,145,164,30,65,142,186,221,226,10,65,56,140,9,138,130,86,61,2,21,0,178,222,220,221,69,103,57,91,39,214,248,223,228,250,192,125,142,126,199,211,2,129,129,0,211,54,220,121,159,246,46,35,158,125,235,108,221,160,127,60,149,2,123,175,130,67,156,20,55,170,233,98,162,103,6,24,109,107,70,169,47,226,100,158,149,117,110,203,43,72,247,21,208,23,129,154,108,169,239,175,71,101,19,112,235,142,212,71,27,121,168,249,40,240,200,179,209,96,14,154,148,3,11,206,124,241,232,242,204,207,156,126,252,139,72,106,92,50,225,150,214,189,230,74,135,207,48,13,153,169,65,149,163,49,9,123,45,154,98,93,29,182,1,73,203,248,138,129,171,189,109,95,2,129,129,0,192,244,111,242,54,184,147,62,13,31,152,17,242,30,175,207,91,11,191,230,112,74,182,228,28,150,96,87,225,86,28,128,17,215,26,119,175,147,29,110,98,206,206,240,2,18,224,56,139,92,144,124,119,140,198,4,175,169,221,247,160,205,134,231,165,66,100,240,204,176,218,44,68,81,142,155,162,224,146,75,102,229,112,134,91,120,90,251,236,211,85,140,131,244,131,86,245,216,219,255,221,40,235,5,131,36,141,175,75,133,119,162,47,179,51,23,5,204,98,103,157,83,197,6,152,88,239,58,2,21,0,138,81,55,111,77,107,16,67,111,18,36,129,205,62,43,84,184,139,229,168 },
   12,448, { 48,130,1,188,2,1,0,2,129,129,0,239,135,135,157,196,106,106,51,10,61,69,133,233,181,242,43,88,180,65,109,22,71,213,237,210,80,77,226,64,215,161,76,8,64,172,56,2,211,59,205,185,109,82,167,206,50,34,181,141,159,93,44,34,128,84,37,21,60,205,33,178,253,162,172,93,190,166,241,179,39,122,35,240,102,177,101,232,218,228,176,49,49,100,46,235,172,190,172,65,139,11,15,48,151,225,199,40,1,236,7,35,232,217,15,78,146,135,236,161,5,74,216,229,242,205,21,223,221,22,81,46,158,180,35,103,240,250,245,2,21,0,137,60,192,137,215,218,196,61,62,219,5,235,31,148,59,203,67,193,170,9,2,129,129,0,173,14,167,116,72,223,44,126,30,216,140,5,106,3,22,145,232,134,141,119,122,50,203,234,119,57,253,11,79,92,93,205,64,80,220,59,188,76,83,139,188,239,24,148,189,85,223,7,91,31,240,145,238,180,209,8,66,193,201,97,95,147,191,158,102,162,2,10,129,23,36,77,214,236,3,187,166,154,226,123,166,83,213,198,180,155,32,125,166,155,135,94,215,36,70,108,11,69,194,120,97,95,249,172,128,92,247,234,147,34,109,164,16,109,1,251,169,29,4,39,18,67,136,3,83,252,130,185,2,129,129,0,152,114,57,34,238,79,89,169,159,40,209,195,0,56,129,158,44,64,56,114,197,80,218,32,136,142,241,172,63,150,177,131,41,244,249,241,55,223,209,78,116,124,167,252,196,199,101,66,127,74,145,176,234,129,227,149,38,218,174,28,105,4,175,193,185,110,10,1,39,135,119,69,40,170,20,116,141,62,171,253,208,44,14,107,174,183,214,66,35,31,172,177,133,5,116,216,31,30,246,154,245,0,108,120,11,46,130,238,12,71,205,169,137,57,92,210,37,12,48,55,1,50,94,85,238,29,143,65,2,20,86,175,111,21,168,191,241,56,15,177,2,60,190,34,120,46,92,181,171,139 },
   13,447, { 48,130,1,187,2,1,0,2,129,129,0,251,127,166,238,49,205,179,99,40,200,108,165,13,225,55,73,231,105,189,36,4,88,4,220,104,99,45,75,25,154,72,3,160,199,93,67,26,83,134,166,162,207,94,172,127,195,31,21,173,218,184,85,13,168,172,184,154,42,32,48,23,118,224,218,149,140,217,50,184,107,112,137,52,227,117,62,249,218,5,83,200,209,28,242,137,136,237,155,179,120,98,140,136,210,169,224,193,196,26,134,22,48,61,152,139,19,152,179,254,148,234,212,148,215,97,167,165,80,202,30,61,73,195,62,91,198,198,117,2,21,0,169,158,255,241,4,123,64,162,114,65,22,72,20,238,99,199,211,28,86,213,2,129,129,0,153,53,34,26,161,48,228,189,199,108,100,88,141,73,235,180,170,127,198,90,196,88,138,172,76,136,73,4,130,86,196,49,60,120,155,128,190,138,239,167,176,181,13,171,58,205,129,213,112,141,79,154,161,99,226,8,68,132,141,220,6,46,144,151,117,72,35,255,160,251,26,91,133,206,91,63,124,173,225,162,24,117,214,83,100,54,219,237,220,113,253,126,246,21,160,8,245,11,16,17,156,171,25,179,65,25,235,255,248,63,19,147,115,223,234,139,208,110,184,12,248,93,166,66,110,40,218,133,2,129,128,102,207,59,164,195,12,38,113,47,17,34,27,164,49,129,74,171,77,200,190,121,33,117,117,133,1,191,46,101,51,86,67,9,105,218,173,29,164,187,151,249,174,189,219,174,119,147,57,171,187,233,193,175,25,192,129,19,241,137,110,133,125,66,175,63,222,243,190,242,254,101,73,91,3,194,117,209,245,107,151,58,109,131,59,137,155,12,87,80,35,232,187,185,236,208,167,105,198,63,151,23,46,151,254,106,96,140,4,174,92,254,171,189,76,64,49,139,253,211,6,195,102,196,5,236,155,144,135,2,20,23,29,234,25,245,208,184,140,215,203,117,126,140,39,27,28,49,49,1,23 },
   14,446, { 48,130,1,186,2,1,0,2,129,129,0,255,24,123,103,154,250,101,242,173,200,116,16,239,211,255,101,69,199,55,206,81,178,125,72,224,48,9,251,137,151,53,163,172,163,253,164,79,225,89,219,43,13,217,234,42,111,213,49,211,104,58,185,181,103,190,92,198,114,100,160,30,224,36,176,0,17,195,16,129,105,207,194,111,230,60,252,190,120,12,200,254,154,4,203,216,144,195,49,200,30,213,178,208,88,102,52,9,251,12,169,172,160,221,14,128,247,105,163,97,193,105,157,21,228,31,124,134,168,168,11,180,126,1,115,128,132,131,227,2,21,0,138,17,87,205,245,252,60,46,252,230,155,99,117,60,248,73,239,107,254,109,2,129,129,0,149,13,118,188,248,94,65,175,193,248,249,77,49,117,111,19,175,108,157,48,33,46,215,252,63,213,49,215,97,16,5,163,27,109,250,109,161,52,129,229,239,21,101,30,55,148,114,128,28,248,246,173,188,32,163,178,178,46,125,95,191,39,167,44,173,184,232,10,1,115,191,107,101,37,247,163,12,168,3,129,178,141,25,139,32,162,10,37,23,176,200,132,125,250,149,174,55,240,244,64,186,49,90,19,219,124,156,195,175,34,64,68,233,224,193,133,222,111,85,153,12,234,139,161,211,28,86,81,2,129,128,113,210,34,125,36,185,190,113,121,78,122,7,30,3,183,219,62,238,97,228,124,93,152,11,148,193,237,236,27,180,30,88,9,95,241,255,14,48,253,50,201,103,141,155,6,107,76,11,175,200,188,9,139,173,129,95,186,171,152,151,196,115,51,106,122,138,251,107,10,145,106,199,161,63,52,84,218,95,63,43,194,17,153,206,163,83,7,194,213,200,41,110,58,188,196,227,160,92,232,240,155,98,47,11,180,242,38,98,88,236,27,72,56,20,199,30,133,187,46,30,128,53,174,182,66,115,195,50,2,19,100,106,40,225,197,76,18,253,247,110,122,124,183,219,0,150,70,117,179 },
   15,447, { 48,130,1,187,2,1,0,2,129,129,0,159,159,253,126,119,159,194,10,206,244,60,186,73,111,41,147,168,128,120,40,114,176,254,112,126,174,10,33,71,145,60,68,149,114,185,253,4,105,242,49,72,21,123,122,134,107,127,149,74,26,180,172,82,129,4,30,21,250,223,122,192,217,167,193,55,189,233,102,110,82,34,203,142,4,19,113,226,254,25,162,65,50,255,122,1,14,83,64,130,141,124,144,41,219,9,157,147,58,182,119,182,107,229,172,132,138,154,113,241,13,10,123,78,209,223,62,90,8,121,168,77,245,189,15,179,189,242,237,2,21,0,137,203,206,172,119,131,81,26,90,85,20,54,89,188,4,148,48,112,46,199,2,129,128,106,231,172,94,109,170,80,207,54,97,82,60,13,22,122,42,217,105,222,34,99,204,233,3,170,11,170,33,152,18,255,49,58,197,89,18,144,217,39,155,62,180,215,7,69,186,198,200,143,49,91,124,255,24,194,194,70,181,212,41,93,185,174,86,188,92,160,138,158,148,160,169,19,111,150,105,2,191,184,152,150,196,176,229,168,251,64,193,206,248,232,135,105,166,218,243,215,126,146,255,245,66,241,204,111,60,129,47,124,186,164,140,62,248,66,139,33,106,179,57,26,240,61,184,167,195,249,80,2,129,129,0,134,200,93,191,127,161,242,29,104,73,56,111,183,42,12,57,149,29,41,63,119,165,41,106,253,6,76,159,250,36,41,241,3,220,186,51,182,67,70,143,33,35,223,137,100,4,120,54,242,201,220,108,74,253,165,54,28,180,31,125,94,157,200,55,95,116,76,12,29,83,39,83,157,5,217,191,150,72,94,131,172,109,206,59,93,70,182,27,206,117,21,220,107,204,233,200,82,125,255,162,25,158,17,34,105,36,63,228,235,159,59,202,232,111,109,34,232,188,33,59,196,78,57,130,146,198,179,172,2,20,89,74,142,238,34,1,150,106,205,217,86,143,138,3,216,41,248,0,197,146 },
   16,446, { 48,130,1,186,2,1,0,2,129,129,0,212,168,137,250,233,139,85,62,58,6,146,167,4,165,56,173,225,85,60,139,70,71,70,209,29,81,126,246,48,94,244,198,41,98,116,154,243,247,71,101,0,41,52,101,45,147,240,46,133,157,42,255,167,192,27,153,44,51,79,46,33,27,70,112,37,60,114,60,56,70,90,242,216,42,18,81,177,14,100,55,10,85,136,209,224,2,173,221,19,82,16,243,62,177,14,198,185,86,37,218,148,188,253,32,2,194,169,44,122,60,19,153,5,84,207,120,116,152,92,249,40,103,107,174,5,181,93,109,2,21,0,204,73,135,219,241,106,41,64,234,212,36,94,163,140,148,129,15,144,237,117,2,129,128,72,46,191,243,184,220,90,224,239,185,226,163,233,24,165,47,104,179,149,195,80,59,80,198,195,241,209,119,212,219,145,55,112,105,192,126,239,199,73,74,162,152,153,58,110,116,224,99,64,18,204,21,217,229,253,254,218,145,59,171,129,188,41,1,78,72,194,149,32,65,139,152,64,168,69,156,96,83,150,231,219,152,223,30,203,159,113,254,129,209,220,14,217,215,72,98,73,191,190,31,187,245,11,182,65,165,60,174,212,121,180,122,13,246,194,107,173,226,47,132,89,88,223,145,235,5,65,189,2,129,128,38,67,152,134,52,22,193,217,37,76,39,146,91,107,210,201,22,113,79,75,0,175,157,47,176,28,142,59,188,56,211,176,1,213,17,17,175,191,20,116,239,78,109,80,97,125,110,170,52,56,35,68,246,232,129,138,149,239,178,220,212,112,135,37,170,17,114,102,203,13,2,167,115,50,193,75,188,170,229,250,62,215,134,214,151,218,210,89,228,39,67,59,121,152,144,208,250,110,47,244,246,114,195,235,181,75,209,196,236,12,251,248,120,168,250,164,159,216,10,46,21,19,229,123,72,185,245,2,2,20,1,237,29,98,16,124,99,115,85,18,194,91,107,129,199,3,37,89,14,89 },
   17,447, { 48,130,1,187,2,1,0,2,129,129,0,237,185,230,129,56,120,124,33,39,63,248,203,49,223,142,139,79,84,227,103,76,202,67,253,217,24,178,69,240,32,167,250,55,199,57,206,48,101,13,77,198,21,103,48,48,119,27,96,40,144,126,58,62,126,115,55,245,60,126,253,202,53,17,136,55,214,201,238,226,126,235,43,44,41,190,76,102,163,222,57,206,81,2,59,152,113,47,203,146,108,27,100,76,74,252,34,44,160,4,161,1,23,124,246,251,41,71,40,55,32,241,112,108,101,179,236,68,108,156,124,24,124,137,47,251,7,58,95,2,21,0,226,47,48,62,192,80,80,166,24,68,161,153,160,223,0,217,56,13,34,113,2,129,128,29,42,222,252,79,137,233,202,66,108,68,46,38,39,121,154,123,216,221,57,193,103,163,122,1,231,100,118,105,126,101,207,107,40,98,123,101,118,154,65,28,216,226,61,88,207,178,109,203,179,239,236,105,116,52,76,247,92,111,88,60,112,231,161,164,241,245,84,89,8,248,78,206,3,43,15,68,50,246,113,132,6,165,203,103,54,22,118,84,74,96,205,97,149,31,189,15,146,65,79,228,234,255,151,179,9,121,158,234,158,241,109,162,12,82,24,42,180,141,156,204,75,59,131,82,123,218,222,2,129,128,23,169,254,250,2,166,78,101,130,153,7,204,198,189,168,90,207,66,0,37,247,209,193,13,176,132,85,132,136,79,248,221,190,202,75,112,149,252,41,199,109,160,0,229,247,130,191,32,110,211,83,75,206,144,16,204,82,124,226,8,165,209,86,234,151,114,201,177,201,163,88,255,111,112,244,112,60,25,175,166,62,160,43,209,195,28,235,170,51,153,206,241,254,168,80,138,80,135,230,34,51,131,45,249,215,36,35,192,191,239,112,219,213,81,237,33,63,13,73,230,188,96,242,136,186,33,232,69,2,21,0,210,5,43,218,51,89,108,167,251,238,91,226,175,138,228,97,129,136,207,252 },
   18,447, { 48,130,1,187,2,1,0,2,129,129,0,135,30,141,149,11,202,134,141,139,155,114,12,153,254,41,221,249,169,169,140,144,4,200,139,208,249,235,94,163,161,206,246,253,180,229,93,56,40,236,208,233,33,255,118,42,18,4,94,133,173,66,145,156,58,106,88,82,51,85,87,184,19,118,37,115,242,73,172,86,46,88,242,223,135,80,82,41,93,41,16,134,117,20,223,211,226,99,109,80,52,203,184,122,196,161,80,133,51,76,130,193,237,8,70,204,35,185,54,89,229,177,28,43,82,161,254,180,131,39,33,28,111,135,207,116,193,59,35,2,21,0,207,52,222,118,235,173,41,78,199,117,243,62,57,7,134,98,95,104,55,81,2,129,128,104,95,177,105,105,155,60,195,20,66,17,101,138,16,42,26,66,26,1,48,111,44,32,76,130,98,248,61,52,253,234,161,110,4,66,152,89,122,19,221,47,110,96,115,167,92,63,186,204,170,95,23,99,123,11,174,223,92,44,94,192,230,74,20,140,94,168,118,18,176,90,26,200,74,85,193,229,5,207,157,236,103,200,208,19,242,240,48,188,176,194,238,201,68,113,236,17,72,246,38,10,68,199,195,5,75,207,141,94,226,247,139,145,30,108,181,113,153,59,242,82,1,216,236,74,87,146,5,2,129,128,82,4,183,242,129,235,111,216,243,49,150,218,137,255,11,53,11,14,120,196,175,164,228,191,76,47,236,23,92,48,191,247,113,161,222,130,139,212,201,23,19,187,83,122,53,56,151,26,95,172,144,164,118,86,167,2,182,110,6,56,227,117,10,180,164,47,224,116,238,44,193,112,92,132,140,67,165,121,35,88,80,227,247,181,58,240,147,60,165,232,219,171,117,157,8,229,167,30,35,2,38,242,117,243,246,30,155,240,248,56,119,227,231,250,143,169,194,104,163,143,183,73,42,184,114,57,103,178,2,21,0,151,250,189,204,127,91,133,111,252,153,87,75,62,25,191,175,232,138,38,91 },
   19,447, { 48,130,1,187,2,1,0,2,129,129,0,135,60,12,143,129,41,64,247,185,41,40,109,228,130,166,207,139,227,11,238,38,1,30,75,199,253,192,1,2,216,68,111,248,167,224,63,191,101,202,151,218,59,4,75,35,242,103,53,5,175,150,45,68,125,26,253,114,92,38,202,47,67,184,87,22,179,243,84,175,235,67,62,72,128,86,88,18,168,68,83,225,160,132,199,165,42,5,106,0,109,236,21,200,254,98,5,212,195,54,181,226,209,59,81,51,93,153,159,115,239,54,43,119,126,224,104,112,51,208,166,69,154,240,170,255,75,242,85,2,21,0,195,58,16,228,169,28,76,15,71,153,176,103,142,254,176,109,241,195,170,177,2,129,128,80,8,20,240,66,56,62,0,213,169,155,102,141,11,224,192,74,172,27,57,9,125,96,48,120,149,215,102,72,147,178,68,31,198,166,239,82,15,158,86,130,62,231,134,129,255,33,244,57,36,251,158,250,46,69,58,182,121,31,239,179,176,96,215,145,142,151,173,84,178,198,204,43,149,59,197,136,91,218,204,10,192,74,6,1,170,76,8,179,15,52,167,189,107,204,203,201,72,52,85,222,172,120,59,228,236,196,61,80,72,94,233,76,56,5,130,106,157,110,73,107,23,87,143,135,215,94,4,2,129,128,79,194,62,7,100,73,52,158,230,21,6,16,224,45,184,110,45,92,145,88,105,44,208,47,159,219,159,244,147,84,243,124,246,180,160,186,16,196,17,252,106,205,184,69,117,219,119,186,16,247,66,98,179,202,141,189,56,127,169,194,175,224,22,209,68,51,3,6,36,244,169,141,136,240,104,32,209,209,174,127,27,249,190,102,164,100,222,247,251,181,7,192,228,135,142,92,225,61,91,205,137,90,253,71,252,231,160,49,211,117,28,64,62,185,52,166,239,237,189,202,165,85,37,6,127,166,30,19,2,21,0,162,22,108,188,108,255,7,34,70,229,114,57,166,77,88,32,161,203,240,223 },
   20,448, { 48,130,1,188,2,1,0,2,129,129,0,204,236,95,83,236,199,55,145,129,237,84,129,156,107,185,15,96,252,25,24,108,203,44,62,110,214,209,111,163,86,134,62,199,69,101,254,154,199,29,59,223,142,166,136,105,46,222,6,115,65,6,129,144,31,127,51,241,7,165,150,140,107,239,98,254,75,33,239,99,75,116,249,103,137,175,173,114,27,209,139,7,7,36,177,225,159,131,196,201,79,71,205,108,0,183,127,192,228,11,52,57,43,186,141,248,109,210,37,67,205,126,13,56,117,203,223,21,187,89,141,63,175,219,155,21,100,228,101,2,21,0,217,139,75,179,68,7,216,147,107,222,204,35,71,37,137,20,221,171,222,177,2,129,129,0,165,73,121,164,15,119,63,51,98,217,130,61,247,82,103,164,174,78,195,146,9,143,160,201,91,131,231,14,103,242,236,144,131,27,29,171,198,208,215,31,5,246,221,222,102,239,60,116,191,151,104,164,185,56,233,69,104,254,192,113,234,0,160,87,151,6,40,62,152,156,134,114,140,29,211,104,168,191,32,87,109,228,113,49,40,127,232,171,106,227,137,238,188,193,119,251,19,112,38,41,159,59,142,14,231,15,241,212,8,164,20,216,72,186,212,218,236,104,12,158,230,155,236,126,236,231,165,32,2,129,128,68,175,126,37,24,149,6,190,130,7,143,200,154,210,99,7,212,181,166,215,61,0,103,69,196,69,97,133,162,106,170,26,42,240,184,243,50,49,216,225,34,65,122,41,88,220,249,78,56,207,163,91,65,10,112,146,41,67,144,102,27,214,4,196,227,34,248,89,32,62,153,130,71,170,159,90,119,55,237,111,117,136,127,109,3,229,33,3,33,243,217,128,197,123,234,253,115,74,124,47,139,81,183,21,16,100,49,40,90,32,203,87,218,61,163,77,57,66,52,171,118,20,133,5,6,77,142,220,2,21,0,154,26,173,23,94,246,207,56,165,93,55,194,169,6,188,210,132,71,251,210 },
};

tOTUOption options[] =
{
  //{"SessionRepository\0", "/sdcard/downloaded\0"},
  {"SessionRepository\0","/storage/sdcard0/downloaded\0"},
  {"LogFile\0", "/data/fota/fotalib.log\0"},
  {"SessionBackupFile\0", "/data/fota/_session_info.sav\0"},
  { NULL, NULL }  /* Make sure to insert this NULL object in order to mark the end of list. */
};

namespace mozilla {
namespace dom {
namespace jrdfota {

tOTUSessionInfo * JrdFotaNative::pSessionInfo = NULL;
bool JrdFotaNative:: bGetNewPackaged = false;

/** **********************************************************
  *                  Class create and release function       *
 *************************************************************/
JrdFotaNative::JrdFotaNative() {
  LOG("enter\n");

  //Init the deviceinfo pointer
  pDeviceInfo = NULL;

  //Init the common timer and time_out
  mCommonTimer = do_CreateInstance("@mozilla.org/timer;1");
  commonTimeOut = 0;

  //For Stop action
  mPauseTimer = do_CreateInstance("@mozilla.org/timer;1");
  mPauseTimeOut = 0;

  //For async handler
  eActionType = eNoAction;
  deliverStatus = eOS_Unknown;
  previousRate = 0;
  bDownloadStarted = false;
  /*Modified by tcl_baijian 2014-03-04 init begin*/
  bInitial = false;
  /*Modified by tcl_baijian 2014-03-04 init end*/
}

JrdFotaNative::~JrdFotaNative() {
  LOG("enter\n");

  _fota_shut_down();

  mCommonTimer = nullptr;
  mPauseTimer = nullptr;
}

/** **********************************************************
  *                  For Child Recv function                                                 *
 *************************************************************/
void
JrdFotaNative::GetNewPackage_Int() {
  LOG("enter\n");

  tOTUStatus res;

  if(true == bGetNewPackaged) {
    /*Modified by tcl_baijian 2014-03-04 just reset begin*/
    _fota_reset();
    /*Modified by tcl_baijian 2014-03-04 just reset end*/
  }

  eActionType = eGetNewPackage;
  LOG("eActionType = eGetNewPackage\n");
  // Init fota lib
  res = _fota_Init();
  if (eOS_Ok == res) {
    //create the device info
    res = _fota_create_device_info();

    if(eOS_Ok == res) {
      //Query the server and check whether there is an update available
      res = otu_request_fileset_info(pDeviceInfo);

      if(eOS_Ok == res) {
        commonTimeOut = 0;
        mCommonTimer->InitWithFuncCallback(_fota_get_new_package_timer_hdlr,
          this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
      }
    }
  }

  if(eOS_Ok != res) {
     _fota_shut_down();
     _fota_send_common_status_response("GetNewPackage", false, res);
     SendResetActionStatus_Int();
  }
   LOG("leave\n");
}

void
JrdFotaNative::Download_Int() {
  LOG("enter\n");

  eActionType = eDownload;
  bDownloadStarted = false;

  if(true == bGetNewPackaged) { //Resume session + Download
    _fota_resume_session();
  }
  else { //Get new session + Resume session + Download
    _fota_get_session_info();
  }
}

void
JrdFotaNative::Pause_Int() {
  LOG("enter\n");
  if(false == bDownloadStarted) {
    //Should shut down, maybe fota_init again when stay get_session_info
    _fota_shut_down();
    _fota_send_common_status_response("Pause", true, eOS_Unknown);
    SendResetActionStatus_Int();
  }
  else {
    tOTUStatus res;

    res = otu_request_download_stop();
    if (eOS_Ok == res) {
      LOG("Pausing download ... ");
      mPauseTimeOut = 0;

      mPauseTimer->InitWithFuncCallback(_fota_pause_timer_hdlr,
         this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
    }
    else { //eOS_AnotherRequestAlreadyOnGoingError
       LOG("[ERROR] Failed to pasuse.\n");
       _fota_send_common_status_response("Pause", false, res);
       SendResetActionStatus_Int();
    }
  }
}

void
JrdFotaNative::Delete_Int() {
  LOG("enter\n");
  bool bRes;

  bRes = _fota_delete_local_update_file();
  /*Modified by tcl_baijian 2014-03-04 remove the _fota_shut_down begin*/
  _fota_send_common_status_response("Delete", bRes, eOS_Failed); //Not need to care the errorType when success.
  SendResetActionStatus_Int();
  if(true == bGetNewPackaged) {/*bGetNewPackaged = false download will be restarted from zero*/
    bGetNewPackaged = false;
  }
  /*Modified by tcl_baijian 2014-03-04 remove the _fota_shut_down end*/
}

void
JrdFotaNative::Install_Int() {
  /*Modified by tcl_baijian 2014-03-17 the storage devices path begin*/
  LOG("Updata file path: %s\n", local_update_file_path_p);
  bool updateFileExist = _fileHandle(local_update_file_path_p, eCheckExist);
  /*Modified by tcl_baijian 2014-03-17 the storage devices path end*/
  if(true == updateFileExist) {
     LOG("Install start\n");

     //Track design: Sure read the update result one time after updated.
     _fileHandle(LOCAL_UPDAT_RESULT_FILE, eCreate);

     nsCOMPtr<nsIRecoveryService> recoveryService = do_CreateInstance("@mozilla.org/recovery-service;1");
     /*Modified by tcl_baijian 2014-03-17 the update package path begin*/
     recoveryService->InstallFotaUpdate(local_update_file_path_p);
     /*Modified by tcl_baijian 2014-03-17 the update package path end*/
  }
  else {
     LOG("[ERROR] Update file not exist\n");
     SendCommonStatusReponse_Int("Install", false, "DiffPackageNotExistError");
     SendResetActionStatus_Int();
  }
}

void
JrdFotaNative::CheckInstallResult_Int() {
  LOG("enter\n");
  int32_t fotaStatus;
  bool updateResultFileExist = _fileHandle(LOCAL_UPDAT_RESULT_FILE, eCheckExist);

  //Step1: check for OS update status after startup
  nsCOMPtr<nsIRecoveryService> recoveryService = do_CreateInstance("@mozilla.org/recovery-service;1");
  recoveryService->GetFotaUpdateStatus(&fotaStatus);

  if (true == updateResultFileExist && fotaStatus != recoveryService->FOTA_UPDATE_UNKNOWN) {
    tOTUStatus res, installStatus = eOS_Error;
    tOTUSessionInfo *pTempSessionInfo = NULL;

    eActionType = eCheckInstallResult;
    if(fotaStatus == recoveryService->FOTA_UPDATE_SUCCESS) {
        LOG("Update success\n");
        installStatus = eOS_Ok;

        //Delete the local update file after install success
        _fota_delete_local_update_file();
    }
    else {
       LOG("[ERROR] Updata fail\n");
    }
    _fileHandle(LOCAL_UPDAT_RESULT_FILE, eRemove);

    //Step2: Report update result to service
    res = _fota_Init();
    if (eOS_Ok == res) {
      //(1): Restore the session info.
      res = otu_restore_session_info(&pTempSessionInfo);
      LOG("Restore the previous Software Details after install:\n");
      _fota_session_info_log(pTempSessionInfo, 1);

      if(eOS_Ok == res && NULL != pTempSessionInfo) {
         //(2): Remove any existing session_info backup file.
         otu_remove_session_info();

         //(3): Report update result to server
         _fota_report_to_otu_server(eOO_Upgrade, installStatus, pTempSessionInfo, 0, 0);
      }
    }
    /*
     *Step3: Notify gaia update result directly when init library or get session_info fail.
     *Of course, we also will reset action status after report service. Please check "_fota_report_to_otu_server"
    */
    if(eOS_Ok != res || NULL == pTempSessionInfo) {
      _fota_shut_down();
    }

    if(eOS_Ok == installStatus) {
      _fota_send_common_status_response("CheckInstallResult", true, eOS_Unknown);
    }
    else {
      _fota_send_common_status_response("CheckInstallResult", false, installStatus);
    }
    /*Wether sucess or fail set action status noAction*/
    SendResetActionStatus_Int();
  }
  else {
     LOG("Have not update result file or read fail\n");
     /*Wether sucess or fail set action status noAction*/
     SendResetActionStatus_Int();
  }
}
/*Modified by tcl_baijian 2014-03-17 selected the storage path begin*/
void
JrdFotaNative::SelectSdcard_Int(unsigned long type,bool immediately)
{
   LOG("the type=%d,immediately=%d",type,immediately);
   if(type == 0)/*internal sdcard*/
   {
        options[0].option_value = "/storage/sdcard0/downloaded\0";
        local_update_file_path_p = "/storage/sdcard0/.downloaded/update.zip";
   }
   else if(type == 1)/*external sdcard*/
   {
       options[0].option_value = "/storage/sdcard1/downloaded\0";
	local_update_file_path_p = "/storage/sdcard1/.downloaded/update.zip";
   }
   local_session_repository_path_p = options[0].option_value;
   if(immediately)/*immediately update*/
   {
      _fota_shut_down();
      _fota_Init();
   }
}
/*Modified by tcl_baijian 2014-03-17 selected the storage path end*/
/** **********************************************************
  *                  For Child Send function                  *
 *************************************************************/
void
JrdFotaNative:: _fota_send_common_status_response(const char* pActionType,
                                              bool isSuccess, tOTUStatus res)
{
   LOG("enter\n");
   const char* errorType = NULL;

   if(false == isSuccess) {
      errorType = _fota_get_error_type_str(res);
   }

   SendCommonStatusReponse_Int(pActionType, isSuccess, errorType);
}

void
JrdFotaNative:: _fota_send_new_package_info_response(tOTUSessionInfo* pSessionInfo)
{
   LOG("enter\n");
   if(NULL != pSessionInfo && NULL != pSessionInfo->file_set && NULL != pSessionInfo->file_set->file_list) {
     //SVN
     string svn;
     _getTagInfoFromDescription(pSessionInfo->file_set->description, "svn", svn);
     int temp = svn.find("_");
     if(string::npos != temp) {
       svn.erase(temp, svn.size()-temp+1);
     }

     //Description
     string description;
     _getTagInfoFromDescription(pSessionInfo->file_set->description, "description", description);

     //Size
     unsigned int i = 0;
     unsigned int total_size = 0;
     tOTUFile *file_list = NULL;
     file_list = pSessionInfo->file_set->file_list;
     for(i = 0; i < pSessionInfo->file_set->file_nb; i++) {
       total_size += file_list->size;
       file_list++;
     }
     LOG("total_size: %d", total_size);

     SendNewPackageInfoReponse_Int(svn.c_str(), total_size, description.c_str());
     SendResetActionStatus_Int();
   }
}
/** **********************************************************
  *                  Jrd Fota function                        *
 *************************************************************/
void
JrdFotaNative::_getTagInfoFromDescription(const char* pDescription, const char* pTagName, string& strTagInfo) {
  if(NULL != pDescription && NULL != pTagName) {
    LOG("Description: %s,  tagName:  %s", pDescription, pTagName);
    int length = strlen(pTagName) + 10;
    char *pTagBegin = new char[length];
    char *pTagEnd = new char[length];
    string strSource(pDescription);

    memset((void*)pTagBegin, 0, length);
    memset((void*)pTagEnd, 0, length);
    //Prepare begin tag
    strcpy(pTagBegin, "<");
    strcat(pTagBegin, pTagName);
    strcat(pTagBegin, ">");
    LOG("TagBegin:  %s", pTagBegin);

    //Prepare end tag
    strcpy(pTagEnd, "</");
    strcat(pTagEnd, pTagName);
    strcat(pTagEnd, ">");
    LOG("TagEnd:  %s", pTagEnd);

    string::size_type posBegin = strSource.find(pTagBegin) + strlen(pTagBegin);
    string::size_type posEnd = strSource.find(pTagEnd);
    if(string::npos != posBegin && string::npos != posEnd) {
      strTagInfo = strSource.substr(posBegin, posEnd - posBegin);
    }
    LOG("strTagInfo:  %s", strTagInfo.c_str());

    //Free buffer
    delete []pTagBegin;
    delete []pTagEnd;
  }
  else {
    LOG("pDescription or pTagName is null");
    MOZ_ASSERT(0);
  }
}

bool
JrdFotaNative::_fileHandle(const char* pFilePath, eFileHandleType handleType) {
  bool res = false;

  if(NULL != pFilePath) {
    LOG("file path: %s\n", pFilePath);
    nsCString path;
    nsCOMPtr<nsIFile> file = do_CreateInstance("@mozilla.org/file/local;1");

    path.SetLength(0);
    path.AssignASCII(pFilePath);
    file->InitWithNativePath(path);

    switch(handleType) {
      case eCheckExist:
        file->Exists(&res);
        break;

      case eCreate:
        file->Create(nsIFile::NORMAL_FILE_TYPE, 0755);
        res = true;
        break;

      case eRemove:
        {
          bool bFileExist = false;
          file->Exists(&bFileExist);
          if (true == bFileExist) {
            file->Remove(true); //Set 'recursive' flag with true to delete directories which are not empty.
            res = true;
          }
        }
        break;

      default :
        MOZ_ASSERT(0);
        break;
    }
  }
  else {
    LOG("file path is null\n");
  }
  return res;
}

const char*
JrdFotaNative::_fota_get_error_type_str(tOTUStatus res) {
  LOG("res: %u\n", res);

  const char* errorStr = NULL;

  if(eOS_ConnectionFailed == res ||
    eOS_NetworkQualityTooBad == res ||
    eOS_Network_Error == res) {
    errorStr = "NetworkError";
  }
  else if(eOS_ServerTooBusy == res ||
    eOS_NoServerAvailableError == res) {
    errorStr = "ServerError";
  }
  else if(eOS_SessionNotExistingError == res) {
    errorStr = "DataError";
  }
  else if(eOS_NoSoftwareAvailableError == res) {
    errorStr = "NotFoundUpdatePackageError";
  }
  else {
    errorStr = "DefaultError";
  }

  LOG("error type: %s\n", errorStr);
  return errorStr;
}

void
JrdFotaNative:: _fota_act_after_report(void) {
   LOG("enter\n");
   if(eDownload == eActionType) {
      if(eOS_Ok == deliverStatus) {
         _fota_check_and_store(pSessionInfo);
      }
      else {
         _fota_shut_down();
         _fota_send_common_status_response("Download", false, deliverStatus);
         SendResetActionStatus_Int();
      }
   }
   else if(eCheckFirmwarm == eActionType) {
      _fota_delete_session(pSessionInfo);
   }
   else if(eCheckInstallResult == eActionType) {
      _fota_shut_down();
      SendResetActionStatus_Int();
   }
}

void
JrdFotaNative::_fota_report_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);

  if(self) {
    tOTUStatus res;

    res = otu_get_request_status();
    self->commonTimeOut++;
    if(!(res<eOS_Ok &&  self->commonTimeOut <= TIME_OUT_MAX_VALUE)) {
      aTimer->Cancel();
      if (self->commonTimeOut > TIME_OUT_MAX_VALUE) {
         LOG("Failed due to time out!\n");
      }
      else if (res != eOS_Ok) {
         LOG("Failed to notify event to server with return code %u!\n", res);
      }
      else {
         LOG("Ok!\n");
      }
      self->_fota_act_after_report();
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

tOTUStatus
JrdFotaNative:: _fota_report_to_otu_server(tOTUOperation operation,
                         tOTUStatus status,
                         tOTUSessionInfo *session_info,
                         tUInt32 statistics_data_size,
                         void *statistics_data)
{
   LOG("enter\n");
   tOTUStatus funcstatus;

   MOZ_ASSERT(NULL != session_info);

   deliverStatus = status;

   funcstatus = otu_notify_status_for_statistics(operation,status,session_info,statistics_data_size,statistics_data);
   if(eOS_Ok == funcstatus) {
      LOG("Notifying server about %u event status ... ",operation);
      commonTimeOut = 0;
      mCommonTimer->InitWithFuncCallback(_fota_report_timer_hdlr,
         this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
   }
   else {
      LOG("Failed to notify event to server with return code %u!\n",funcstatus);
      _fota_act_after_report();
   }
   return funcstatus;
}


tOTUStatus
JrdFotaNative::_fota_Init(void) {
  LOG("enter\n");
  /*Added by tcl_baijian 2014-03-04 init only once begin*/
  if(bInitial == true)
  {
     LOG("The so lib is already initialed\n");
     return eOS_Ok;
  }
  /*Added by tcl_baijian 2014-03-04 init only once end*/
  tOTUStatus res = eOS_Ok;

  OTU_DLL_KEY_CBF_T  callback_arr[OTU_DLL_KEY_CBF_ARRAY_MAX_SIZE];

  memset(callback_arr, 0, sizeof(callback_arr));

  callback_arr[eECB_EncryptStringCB] = _encrypt_data;
  callback_arr[eECB_GetKeyIdCB] = _get_key_id;
  callback_arr[eECB_GetVersionIdCB] = _get_client_version_id;

  /**
   * The three parameter of otu_init_lib: [log_mode]: Activate or deactivate logging into the log file.
   * The four parameter of otu_init_lib: [debug_mode]: Activate or deactivate the debug verbosity in log file.
  */
  /*Modified by tcl_baijian 2014-03-04 the new lib otu_init changed begin*/
  if (FOTA_DEBUG) {
    res = otu_init(callback_arr, OTU_DLL_KEY_CBF_ARRAY_MAX_SIZE, 1, 1, eCT_TCT_Android_3G_Mobile_Build_In_Fota, options);
  }
  else {
    res = otu_init(callback_arr, OTU_DLL_KEY_CBF_ARRAY_MAX_SIZE, 0, 0, eCT_TCT_Android_3G_Mobile_Build_In_Fota, options);
  }
  /*Modified by tcl_baijian 2014-03-04 the new lib otu_init changed end*/
  // Truncate the log file.
  otu_remove_log();
  /*Added by tcl_baijian 2014-03-04 when init finished set bInitial begin*/
  if(res == eOS_Ok)
  {
      bInitial = true;
  }
  /*Added by tcl_baijian 2014-03-04 when init finished set bInitial end*/
  LOG("res: %u\n",res);
  return res;
}

void
JrdFotaNative::_fota_shut_down(void) {
  LOG("enter\n");
  /*Modified by tcl_baijian 2014-03-04 do not shutdown once more begin*/
  if(bInitial == false)
  {
     LOG("The lib is already shut down\n");
     return;
  }
  if (mCommonTimer) {
    mCommonTimer->Cancel();
  }

  if (mPauseTimer) {
    mPauseTimer->Cancel();
  }

  mPauseTimeOut = 0;
  commonTimeOut = 0;

  //Reset the deviceinfo, sessioninfo pointer
  pDeviceInfo = NULL;
  pSessionInfo = NULL;

  //For async handler
  bGetNewPackaged = false;
  eActionType = eNoAction;
  deliverStatus = eOS_Unknown;
  previousRate = 0;
  bDownloadStarted = false;
  otu_shutdown();
  bInitial = false;
  /*Modified by tcl_baijian 2014-03-04 do not shutdown once more end*/
}
/*Added by tcl_baijian 2014-03-04 lib reset begin*/
void JrdFotaNative::_fota_reset(void)
{
   LOG("enter\n");
  if (mCommonTimer) {
    mCommonTimer->Cancel();
  }

  if (mPauseTimer) {
    mPauseTimer->Cancel();
  }

  mPauseTimeOut = 0;
  commonTimeOut = 0;

  //Reset the deviceinfo, sessioninfo pointer
  pDeviceInfo = NULL;
  pSessionInfo = NULL;

  //For async handler
  bGetNewPackaged = false;
  eActionType = eNoAction;
  deliverStatus = eOS_Unknown;
  previousRate = 0;
  bDownloadStarted = false;
  otu_reset();
  LOG("leave\n");
}
/*Added by tcl_baijian 2014-03-04 lib reset end*/
tOTUStatus
JrdFotaNative::_fota_get_device_current_params(char  *pSwVersion, char *pImei, char *pCmRefer)
{
  LOG("enter\n");
  tOTUStatus res = eOS_Ok;

  //Get IMEI
  if(NULL != pImei) {
    nsCOMPtr<nsIJrdNvAccess> nvManager = do_CreateInstance("@jrdcom.com/JrdNvAccess;1");
    uint32_t count = 0;
    uint8_t *pValue = NULL;
    unsigned char i, digit, imei_bcd_len = 0;
    char imei_ascii[DEVICE_PARAM_STR_MAX_LENGTH+1];

    //NV_UE_IMEI_I  = 0
    nvManager->ReadNvitem(0, &count, &pValue);

    if(NULL != pValue && count == 15) {
        memset( imei_ascii, 0, sizeof(imei_ascii));
        /*fixed bug#626492 pValue is ascii,needn't change*/
        memcpy(imei_ascii,pValue,count);
        /*
        for( i = 0; i < count; i++ ) {
          imei_ascii[i] = pValue[ i ] + '0';
        }*/
        memcpy(pImei, imei_ascii, DEVICE_PARAM_STR_MAX_LENGTH+1);
    }
    LOG("pImei: %s\n", pImei);
  }
  /*Modified by tcl baijian version rule changed 2014-02-21 begin*/
  //Get current version
  if(res == eOS_Ok && NULL != pSwVersion) {
    nsCString path;
    char buf[DEVICE_PARAM_STR_MAX_LENGTH] = {0};
    uint32_t count = 0;
    uint32_t versionLength = 0;
    nsCOMPtr<nsIFile> file = do_CreateInstance("@mozilla.org/file/local;1");
    nsCOMPtr<nsIFileInputStream> stream = do_CreateInstance("@mozilla.org/network/file-input-stream;1");

    path.SetLength(0);
    path.AssignASCII("/custpack/custpack.ver");
    file->InitWithNativePath(path);

    memset((void*)buf, 0, sizeof(buf));
    stream->Init(file, -1, -1, 0);
    stream->Read(buf, sizeof(buf)-1, &count);
    LOG("System version: %s, count: %d\n", buf, count);
    versionLength = count;
    if(versionLength >= 11 && versionLength < DEVICE_PARAM_STR_MAX_LENGTH) {
       /**
       *main version:bit9,10,11;
       *peso and sub-perso version:bit5,6,7,8
       */
      strncpy(pSwVersion, &buf[8], 3);
      strncat(pSwVersion, &buf[4], 4);
      pSwVersion[7] = '\0';
      LOG("sw version: %s\n", pSwVersion);
    }
    else {
      res = eOS_Error;
      LOG("[ERROR]  Get software version fail\n");
    }
  }
  /*Modified by tcl baijian version rule changed 2014-02-21 end*/

  //Get commercial reference
  if(res == eOS_Ok && NULL != pCmRefer) {
    nsCOMPtr<nsIJrdNvAccess> nvManager = do_CreateInstance("@jrdcom.com/JrdNvAccess;1");
    uint32_t count = 0;
    uint8_t *pValue = NULL;
    uint32_t i = 0, j = 0;
    /*commercial reference item is 55*/
    nvManager->ReadNvitem(55/*50001*/, &count, &pValue);
    /*Modified by tcl_baijian 2014-03-04 cu-ref length:count begin*/
    if(NULL != pValue && count > 0 && count <=20) {
      //INFO_COMM_REF_I: {index: 55, length: 20, ascii: true}
      for(i = 0; i < count; i++){
        //Break directly when not acsii character
        if(pValue[i] <32 || pValue[i] > 126) {
          break;
        }
        pCmRefer[j] = pValue[i];
        j++;
      }
      LOG("Commercial reference: %s\n", pCmRefer);
    }
    /*Modified by tcl_baijian 2014-03-04 cu-ref length:count end*/
    if(0 == strlen(pCmRefer)) {
      res = eOS_Error;
      LOG("[ERROR]  Get commercial reference fail\n");
    }
  }
  return res;
}

tOTUStatus
JrdFotaNative::_fota_create_device_info(void) {
  LOG("enter\n");
  tOTUStatus res = eOS_Ok;

  /* RS
     Here we hard coded the value for test. But in real client, you MUST read these information
     from the phone.
  */

  char current_version[DEVICE_PARAM_STR_MAX_LENGTH+1] = {0};
  char sw_imei[DEVICE_PARAM_STR_MAX_LENGTH+1] = {0};
  char sw_com_ref[DEVICE_PARAM_STR_MAX_LENGTH+1] = {0};

  res = _fota_get_device_current_params(current_version, sw_imei, sw_com_ref);
/*
  //Test device params
  char current_version[]="165FP00";
  char sw_imei[]="861667010105942";
  char sw_com_ref[]="4012A_FOTAYU";
*/
  if(eOS_Ok == res) {
    // RS - These 3 can be kept hard coded to this value.
    char sw_model[]="FIRMWARE";
    tOTUUpgradeMode update_mode=eOUM_Incremental;
    tOTUClientType client_type = eCT_TCT_Android_3G_Mobile_Build_In_Fota;
    /*Modified by tcl_baijian 2014-03-04 new lib interface changed begin*/
    if(NULL == pDeviceInfo) {
      pDeviceInfo = otu_create_device_info(sw_imei,sw_model,sw_com_ref,current_version, update_mode, client_type,NULL);
      if(NULL == pDeviceInfo) {
        res = eOS_Error;
      }
    }
    /*Modified by tcl_baijian 2014-03-04 new lib interface changed begin*/
  }
  LOG("res: %u\n",res);
  return res;
}

void
JrdFotaNative::_fota_get_new_package_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);
  tGOTUSpecialOperation special_op;

  if(self) {
    tOTUStatus res;

    res = otu_get_request_status();
    LOG("res: %d\n", res);
    self->commonTimeOut++;
    if(!(res<eOS_Ok &&  self->commonTimeOut <= TIME_OUT_MAX_VALUE)) {
      aTimer->Cancel();
      if(self->commonTimeOut > TIME_OUT_MAX_VALUE) {
        self->_fota_shut_down();
        self->_fota_send_common_status_response("GetNewPackage", false, eOS_TimeOut);
        self->SendResetActionStatus_Int();
      }
      else {
        if(eOS_Ok == res) {
          res = otu_get_file_list(&self->pSessionInfo);
          if(eOS_Ok == res) {
            MOZ_ASSERT(NULL != self->pSessionInfo);
            self->_fota_session_info_log(self->pSessionInfo, 1);

            //Delete the old sessions
            if(false == self->_fota_delete_old_sessions(self->pSessionInfo)) {
              //Notify the version data to gaia
              self->_fota_send_new_package_info_response(self->pSessionInfo);

              //Mark this getting status
              self->bGetNewPackaged = true;
            }
          }
          else {
            LOG("[ERROR] Failed to retrieve file list after successfull request!\n");
          }
        }

        if(eOS_Ok != res) {
          self->_fota_shut_down();
          self->_fota_send_common_status_response("GetNewPackage", false, res);
          self->SendResetActionStatus_Int();
        }
      }
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

bool
JrdFotaNative::_fota_delete_old_sessions(tOTUSessionInfo *pNewSessionInfo) {
  LOG("enter\n");
  tOTUStatus res;
  unsigned int nb_session = 0;
  tOTUSessionInfo *session_info_list = NULL;
  bool bRet = false;

  if(NULL != pNewSessionInfo &&
    NULL != pNewSessionInfo->device_info &&
    NULL != pNewSessionInfo->file_set)
  {
    res = otu_get_session_list(&nb_session,&session_info_list);
    _fota_session_info_log(session_info_list, nb_session);

    if ((eOS_Ok == res) && (nb_session > 0)) {
      unsigned int i=0;
      for (i=0; i < nb_session; i++) {
        tOTUSessionInfo *local_session_info;
        local_session_info=&session_info_list[i];

        if (NULL != local_session_info &&
          NULL != local_session_info->device_info &&
          NULL != local_session_info->file_set &&
          strcmp(pNewSessionInfo->device_info->file_set_type, local_session_info->device_info->file_set_type) == 0 &&
          strcmp(pNewSessionInfo->device_info->commercial_reference, local_session_info->device_info->commercial_reference) == 0 &&
          pNewSessionInfo->device_info->upgrade_mode == local_session_info->device_info->upgrade_mode &&
          pNewSessionInfo->device_info->client_type == local_session_info->device_info->client_type &&
          strcmp(pNewSessionInfo->device_info->version_code, local_session_info->device_info->version_code)==0 &&
          strcmp(pNewSessionInfo->file_set->to_version_code, local_session_info->file_set->to_version_code) > 0)
        {
          bRet = true;
          _fota_delete_session(local_session_info);
          break;
        }
      }
    }
  }
  return bRet;
}

void
JrdFotaNative::_fota_download_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);

  if(self) {
     int dlstatus = 0;
     tOTUStatus res;
     tUInt32 total_size;
     tUInt32 downloaded_size;

     dlstatus = otu_get_request_completion_status();
     res = otu_get_request_status();
     otu_downloaded_byte(&total_size,&downloaded_size);

     if(eOS_Ok == res && 1000 == dlstatus) {
       aTimer->Cancel();
       /**
        * Report download status to server.
        * And here don't need to set timer, because now we don't shut down library and don't care the report result.
       */
       LOG("SUAPP: Downloading ... %u/%u bytes (%3.1f%%) @ %.2f bytes/s                                \n",
          downloaded_size, total_size, (double) dlstatus/10.,otu_download_speed());

       LOG("\nDownload succeeded. Terminated.\n");

       //Dispatch completion rate to gaia
       self->SendDownloadCompleteRateReponse_Int(dlstatus/10);

       self->_fota_report_to_otu_server(eOO_Download, eOS_Ok, self->pSessionInfo, 0, 0);
     }

     if (res > eOS_FatalError) {
       LOG("[ERROR] An unrecoverable error occured: %u\n",res);

       aTimer->Cancel();

       //Report download status to server.
       self->_fota_report_to_otu_server(eOO_Download, res, self->pSessionInfo, 0, 0);
     }

     LOG("SUAPP: Downloading ... %u/%u bytes (%3.1f%%) @ %.2f bytes/s                 \r",
       downloaded_size, total_size, (double) dlstatus/10.,otu_download_speed());

     //Dispatch completion rate to gaia
     if(self->previousRate != dlstatus/10) {
       self->SendDownloadCompleteRateReponse_Int(dlstatus/10);
       self->previousRate = dlstatus/10;
     }
  }
  else if(aTimer) {
     aTimer->Cancel();
  }
}

void
JrdFotaNative::_fota_download_start(void) {
   LOG("enter\n");
   tOTUStatus res;

   MOZ_ASSERT(NULL != pSessionInfo);
   res = otu_request_download_start(pSessionInfo);
   if (eOS_Ok == res) {
     bDownloadStarted = true;
     previousRate = 0;
     mCommonTimer->InitWithFuncCallback(_fota_download_timer_hdlr,
       this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
   }
   else {
     //Report download status to server.
     _fota_report_to_otu_server(eOO_Download, res, pSessionInfo, 0, 0);
   }
}

void
JrdFotaNative::_fota_resume_session_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);

  if(self) {
    tOTUStatus res;

    res = otu_get_request_status();
    LOG("res: %d\n", res);
    if(res >= eOS_Ok) {
      aTimer->Cancel();
      if(eOS_Ok == res) {
        int dlstatus = 0;

        dlstatus = otu_get_request_completion_status();
        if (1000 == dlstatus) {
          /**
           * Download already completed!
           * The demo code of the lib document hava handled this case.
           * But gaia hava saved the completion rate of the previous download.
           * This case exist??
          */
          self->SendDownloadCompleteRateReponse_Int(dlstatus/10);

          self->_fota_check_and_store(self->pSessionInfo);
        }
        else {
          LOG("Session resume success and need to be completed\n");
          self->_fota_download_start();
        }
      }
      else {
         LOG("[ERROR] Failed to resume session due to error %u.\n",res);
         self->_fota_download_start();
      }
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

void
JrdFotaNative::_fota_resume_session(void) {
  LOG("enter\n");
  tOTUStatus res;

  MOZ_ASSERT(NULL != pSessionInfo);
  if(eOS_Ok == otu_get_session_availability(pSessionInfo)) {
    res = otu_request_session_resume(pSessionInfo);
    if (eOS_Ok == res) {
      mCommonTimer->InitWithFuncCallback(_fota_resume_session_timer_hdlr,
        this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
    }
    else {
       LOG("[ERROR] Failed to resume session due to error %u.\n",res);
       _fota_download_start();
    }
  }
  else {
     LOG("No session on disk has been found");
     _fota_download_start();
  }
}

void
JrdFotaNative::_fota_get_session_info_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);
  tGOTUSpecialOperation special_op;

  if(self) {
    tOTUStatus res;

    res = otu_get_request_status();
    LOG("res: %d\n", res);
    self->commonTimeOut++;
    if(!(res < eOS_Ok &&  self->commonTimeOut <= TIME_OUT_MAX_VALUE)) {
      aTimer->Cancel();
      if(self->commonTimeOut > TIME_OUT_MAX_VALUE) {
        self->_fota_shut_down();
        self->_fota_send_common_status_response("Download", false, eOS_TimeOut);
        self->SendResetActionStatus_Int();
      }
      else {
        if(eOS_Ok == res) {
          res = otu_get_file_list(&self->pSessionInfo);

          if(eOS_Ok == res) {
            MOZ_ASSERT(NULL != self->pSessionInfo);
            self->_fota_session_info_log(self->pSessionInfo, 1);

            //Delete the old sessions
            if(false == self->_fota_delete_old_sessions(self->pSessionInfo)) {
              //Resume session
              self->_fota_resume_session();

              //Mark this getting status
              self->bGetNewPackaged = true;
            }
          }
        }

        if(eOS_Ok != res) {
           self->_fota_shut_down();
           self->_fota_send_common_status_response("Download", false, res);
           self->SendResetActionStatus_Int();
        }
      }
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

void
JrdFotaNative::_fota_get_session_info(void) {
  LOG("enter\n");
  tOTUStatus res;

  // Init fota lib
  res = _fota_Init();
  if (eOS_Ok == res) {
    //create the device info
    res = _fota_create_device_info();

    if(eOS_Ok == res) {
      //Query the server and check whether there is an update available
      res = otu_request_fileset_info(pDeviceInfo);

      if(eOS_Ok == res) {
        commonTimeOut = 0;
        mCommonTimer->InitWithFuncCallback(_fota_get_session_info_timer_hdlr,
          this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
      }
    }
  }

  if(eOS_Ok != res) {
     _fota_shut_down();
     _fota_send_common_status_response("Download", false, res);
     SendResetActionStatus_Int();
  }
}

tOTUStatus
JrdFotaNative::_fota_store_downloaded_file(tOTUSessionInfo* pSessionInfo) {
  LOG("enter\n");
  tOTUStatus res = eOS_Ok;
  nsCString path;

  MOZ_ASSERT(NULL != pSessionInfo);

  if(NULL != pSessionInfo->file_set &&
    pSessionInfo->file_set->file_mask &&
    pSessionInfo->file_set->file_list)
  {
    nsCOMPtr<nsIFile> file = do_CreateInstance("@mozilla.org/file/local;1");
    nsCOMPtr<nsIFileOutputStream> stream = do_CreateInstance("@mozilla.org/network/file-output-stream;1");

    unsigned int i = 0;
    /**
    * Create "/sdcard/.downloaded/update.zip" file
    * Save all files to SDCard.
    */
    path.SetLength(0);
    /*Modified by tcl_baijian 2014-03-17 update package path begin*/
    path.AssignASCII(local_update_file_path_p);
    /*Modified by tcl_baijian 2014-03-17 update package path end*/
    file->InitWithNativePath(path);
    file->Create(nsIFile::NORMAL_FILE_TYPE, 0755);

    for (i = 0; i < pSessionInfo->file_set->file_nb; ++i) {
      unsigned long sizecounter = 0;
      unsigned int databyte = 0;
      uint32_t count = 0;
      char *buf = NULL;

      //This file was masked out! Skipping it as we didn't downloaded  the corresponding chunks.
      if (pSessionInfo->file_set->file_mask[i] == 0) {
        continue;
      }

      // Write content to the file
      stream->Init(file, -1, -1, 0);
      buf = (char*)malloc(pSessionInfo->file_set->file_list[i].size + 1);
      memset((void*)buf, 0 , pSessionInfo->file_set->file_list[i].size + 1);
      for (databyte=otu_get_first_sw_byte(i); databyte != UNDEFINED;
        databyte=otu_get_next_sw_byte(),sizecounter++) {
        buf[sizecounter] = (char)databyte;
      }
      stream->Write(buf, sizecounter, &count);

      //Check the content size
      if (sizecounter != pSessionInfo->file_set->file_list[i].size) {
        free((void*)buf);
        res = eOS_CorruptedFileError;
        break;
      }

      //Release the temp buffer
      free((void*)buf);
    }
  }
  else {
    res = eOS_Error;
  }
  return res;
}

void
JrdFotaNative:: _fota_act_after_delete_session(tOTUStatus delete_status) {
   LOG("enter\n");
   tOTUStatus res;
   if(eCheckFirmwarm == eActionType) {
     if(eOS_Ok == deliverStatus) {
       res=otu_backup_session_info(pSessionInfo);
       if (eOS_Ok!=res) {
         LOG("[ERROR] Failed to execute otu_backup_session_info with error code: %u.\n",res);
       }
       else {
         LOG("backup session info success\n");
       }
       _fota_shut_down();
       _fota_send_common_status_response("CheckFirmwarm", true, eOS_Unknown);
       SendResetActionStatus_Int();
     }
     else {
       _fota_shut_down();
       _fota_send_common_status_response("CheckFirmwarm", false, deliverStatus);
       SendResetActionStatus_Int();
     }
   }
   else if(eGetNewPackage == eActionType) {
     if(!(eOS_Ok == delete_status && true == _fota_delete_old_sessions(pSessionInfo))) {
       //Notify the version data to gaia
       _fota_send_new_package_info_response(pSessionInfo);

       //Mark this getting status
       bGetNewPackaged = true;
     }
   }
   else if(eDownload == eActionType) {
     if(!(eOS_Ok == delete_status && true == _fota_delete_old_sessions(pSessionInfo))) {
        //Resume session
        _fota_resume_session();

        //Mark this getting status
        bGetNewPackaged = true;
     }
   }
}

void
JrdFotaNative::_fota_delete_session_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  tOTUStatus res;
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);

  if(self) {
    res = otu_get_request_status();
    self->commonTimeOut++;
    if(!(res < eOS_Ok &&  self->commonTimeOut <= TIME_OUT_MAX_VALUE)) {
      aTimer->Cancel();

      if (self->commonTimeOut > TIME_OUT_MAX_VALUE) {
          res = eOS_TimeOut;
          LOG("Failed due to time out!\n");
      }
      else if (res != eOS_Ok) {
         LOG("[ERROR] Failed with returned code %u!\n", res);
      }
      else {
         LOG("Ok!\n");
      }
      self->_fota_act_after_delete_session(res);
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

void
JrdFotaNative::_fota_delete_session(tOTUSessionInfo* session_info) {
   tOTUStatus res;

   MOZ_ASSERT(NULL != session_info);
   res=otu_request_session_delete(session_info);
   if (eOS_Ok==res) {
     LOG("Deleting session ...");

     commonTimeOut = 0;
     mCommonTimer->InitWithFuncCallback(_fota_delete_session_timer_hdlr,
       this, TIME_INTERVAL_VALUE, nsITimer::TYPE_REPEATING_SLACK);
   }
   else {
     _fota_act_after_delete_session(res);
   }
}

void
JrdFotaNative::_fota_check_and_store(tOTUSessionInfo* session_info) {
   MOZ_ASSERT(NULL != session_info);
   eActionType = eCheckFirmwarm;
   deliverStatus = _fota_store_downloaded_file(session_info);
   if(eOS_Ok != deliverStatus) {
      _fota_report_to_otu_server(eOO_Upgrade, deliverStatus, session_info,0,0);
   }
   else {
      _fota_delete_session(session_info);
   }
}

void
JrdFotaNative::_fota_pause_timer_hdlr(nsITimer *aTimer, void *aClosure) {
  JrdFotaNative *self = reinterpret_cast<JrdFotaNative *>(aClosure);

  if(self) {
    tOTUStatus res;

    res = otu_get_request_status();
    self->mPauseTimeOut++;
    if(!(res != eOS_Paused && res < eOS_Ok && self->mPauseTimeOut <= TIME_OUT_MAX_VALUE)) {
      aTimer->Cancel();
      if (self->mPauseTimeOut > TIME_OUT_MAX_VALUE) {
         LOG("Failed due to time out!\n");
         self->_fota_send_common_status_response("Pause", false, eOS_TimeOut);
         self->SendResetActionStatus_Int();
      }
      else {
        if (res != eOS_Paused) {
           LOG("Failed with returned code %u!\n",res);
           self->_fota_send_common_status_response("Pause", false, res);
           self->SendResetActionStatus_Int();
        }
        else {
           LOG("Ok!\n");
           //Only stop the previous download timer. The next download action can be runned quickly.
           if (self->mCommonTimer) {
             self->mCommonTimer->Cancel();
           }

           self->_fota_send_common_status_response("Pause", true, eOS_Unknown);
           self->SendResetActionStatus_Int();
        }
      }
    }
  }
  else if(aTimer) {
    aTimer->Cancel();
  }
}

bool
JrdFotaNative::_fota_delete_local_update_file(void) {
  LOG("enter\n");
  /*Modified by tcl_baijian 2014-03-17 update package and session path begin*/
  const char* pathStr[2] = {local_update_file_path_p, local_session_repository_path_p};
  /*Modified by tcl_baijian 2014-03-17 update package and session path end*/
  bool res = false;

  //Delete local session repository and local update file
  for(int i = 0; i < 2; i++) {
    res = _fileHandle(pathStr[i], eRemove);
  }
  return res;
}
/** **********************************************************
  *                  debug log function                       *
 *************************************************************/
void
JrdFotaNative::_fota_device_info_log(tOTUDeviceInfo* pDeviceInfo) {
  if(NULL != pDeviceInfo) {
    LOG("{ device_id: %s, file_set_type: %s, commercial_reference: %s, version_code: %s, upgrade_mode: %d, client_type: %d }\n",
      pDeviceInfo->device_id,
      pDeviceInfo->file_set_type,
      pDeviceInfo->commercial_reference,
      pDeviceInfo->version_code,
      pDeviceInfo->upgrade_mode,
      pDeviceInfo->client_type);
  }
}

void
JrdFotaNative::_fota_file_info_log(tOTUFile* pFileInfo) {
  if(NULL != pFileInfo) {
    LOG("{ file_id: %d, file_name: %s, size: %d, nb_chunk: %d, version: %s, checksum: %s }\n",
      pFileInfo->file_id,
      pFileInfo->file_name,
      pFileInfo->size,
      pFileInfo->nb_chunk,
      pFileInfo->version,
      pFileInfo->checksum);
  }
}

void
JrdFotaNative::_fota_file_set_log(tOTUFileSet* pFileSet) {
  if(NULL != pFileSet) {
    unsigned int i = 0;
    LOG("{ file_nb: %d, description: %s, to_version_code: %s, upgrade_mode: %d, from_version_code: %s, file_mask: %s, internal_version: %d }\n",
      pFileSet->file_nb,
      pFileSet->description,
      pFileSet->to_version_code,
      pFileSet->upgrade_mode,
      pFileSet->from_version_code,
      pFileSet->file_mask,
      pFileSet->internal_version);
    for(i = 0; i < pFileSet->file_nb; i++)
    {
      _fota_file_info_log(pFileSet->file_list+i);
    }
  }
}

void
JrdFotaNative::_fota_session_info_log(tOTUSessionInfo* pSessionInfo, int nb_session) {
  if(NULL != pSessionInfo) {
    LOG("{ session number: %d \n\n", nb_session);
    int i = 0;
    for(i = 0; i < nb_session; i++) {
      LOG("{ name: %s }\n", pSessionInfo[i].name);
      _fota_device_info_log(pSessionInfo[i].device_info);
      _fota_file_set_log(pSessionInfo[i].file_set);
      LOG("\n");
    }
    LOG("}\n");
  }
}
/** **********************************************************
  *                  Call back functions for otu_init         *
 *************************************************************/
/**
 * The first one will compute the encrypt data of input data, please do exactly
 * the same, or the server will return 'eOS_ClientToolKeyError'.
*/
int
JrdFotaNative::_encrypt_data(tByte *pOutStr, tUInt32 *pOutStrLen,
                    tUInt32 max_out_len, const tByte *pInStr, tUInt32 in_str_len)
{
  LOG("enter\n");
  DSA *dsa;
  int res=0;
  const tByte *p_private_key;
  int client_private_key_len;

  tByte digest[SHA_DIGEST_LENGTH];

  _selected_key_index++;

  if (_selected_key_index>=(MAX_PRIV_KEYS-1)) {
    _selected_key_index=0;
  }

  p_private_key=(tByte *) &_client_private_keys_info[_selected_key_index].key_content[0];
  client_private_key_len=_client_private_keys_info[_selected_key_index].key_len;

  // Computing data digest.
  SHA1(pInStr, in_str_len, &digest[0]);

  // Restoring private key.
  dsa=d2i_DSAPrivateKey(0, &p_private_key, client_private_key_len);

  if (dsa == NULL) {
    LOG("[ERROR] due to dsa is NULL\n");
    return 1;
  }

  // Making sure all the signature data has been initialized.
  memset((char *)pOutStr,0xFF,max_out_len);

  if (1!=DSA_sign(0, &digest[0], SHA_DIGEST_LENGTH, pOutStr, pOutStrLen, dsa)) {
    LOG("[ERROR] due to DSA_sign\n");
    res=1;
  }

  DSA_free(dsa);

  return res;
}
/**
 * We will send you a group of private keys. Each key has an unique ID. So, the
 * second call back function is used to get the ID of the key which you used in
 * the first call back function.
*/
int
JrdFotaNative::_get_key_id(tByte *pOutStr, tUInt32 *pOutStrLen,
                 tUInt32 max_out_len, const tByte *pInStr, tUInt32 in_str_len)
{
  *pOutStrLen = _client_private_keys_info[_selected_key_index].key_id;
  LOG("OutStrLen = %d\n", *pOutStrLen);
  return 0;
}
/**
 * The Third call back function is used  to get the client version ID. It is the
 * version of your client (If you do not have the integar version ID, you can put
 * the build date here, e.g., 20120520).
*/
int
JrdFotaNative::_get_client_version_id(tByte *pOutStr, tUInt32 *pOutStrLen,
                 tUInt32 max_out_len, const tByte *pInStr, tUInt32 in_str_len)
{
  *pOutStrLen = otu_int32_build_id();
  LOG("OutStrLen = %d\n", *pOutStrLen);
  return 0;
}

} // namespace jrdfota
} // namespace dom
} // namespace mozilla mozilla
