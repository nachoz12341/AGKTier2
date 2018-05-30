# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := AGKCurl
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../common/include \
    $(LOCAL_PATH)/../../../common \
    $(LOCAL_PATH)/../../../common/Collision \
    $(LOCAL_PATH)/libpng \
    $(LOCAL_PATH)/libjpeg \
    $(LOCAL_PATH)/../../../zlib \
    $(LOCAL_PATH)/../../../zxing \
    $(LOCAL_PATH)/../../../curl \
    $(LOCAL_PATH)/../../../curl/include \
    $(LOCAL_PATH)/../../../curl/axTLS \
    $(LOCAL_PATH)/../../../giflib \
    $(LOCAL_PATH)/../../../json \
    $(LOCAL_PATH)/../../../assimp \
    $(LOCAL_PATH)/../../../bullet \
    $(LOCAL_PATH)/../../../bullet/BulletCollision/CollisionShapes
LOCAL_SRC_FILES := ../../../curl/amigaos.c \
    ../../../curl/asyn-ares.c \
    ../../../curl/asyn-thread.c \
    ../../../curl/base64.c \
    ../../../curl/conncache.c \
    ../../../curl/connect.c \
    ../../../curl/content_encoding.c \
    ../../../curl/cookie.c \
    ../../../curl/curl_addrinfo.c \
    ../../../curl/curl_des.c \
    ../../../curl/curl_endian.c \
    ../../../curl/curl_fnmatch.c \
    ../../../curl/curl_gethostname.c \
    ../../../curl/curl_gssapi.c \
    ../../../curl/curl_memrchr.c \
    ../../../curl/curl_multibyte.c \
    ../../../curl/curl_ntlm_core.c \
    ../../../curl/curl_ntlm_wb.c \
    ../../../curl/curl_rtmp.c \
    ../../../curl/curl_sasl.c \
    ../../../curl/curl_sspi.c \
    ../../../curl/curl_threads.c \
    ../../../curl/dict.c \
    ../../../curl/dotdot.c \
    ../../../curl/easy.c \
    ../../../curl/escape.c \
    ../../../curl/file.c \
    ../../../curl/fileinfo.c \
    ../../../curl/formdata.c \
    ../../../curl/ftp.c \
    ../../../curl/ftplistparser.c \
    ../../../curl/getenv.c \
    ../../../curl/getinfo.c \
    ../../../curl/gopher.c \
    ../../../curl/hash.c \
    ../../../curl/hmac.c \
    ../../../curl/hostasyn.c \
    ../../../curl/hostcheck.c \
    ../../../curl/hostip.c \
    ../../../curl/hostip4.c \
    ../../../curl/hostip6.c \
    ../../../curl/hostsyn.c \
    ../../../curl/http.c \
    ../../../curl/http_chunks.c \
    ../../../curl/http_digest.c \
    ../../../curl/http_negotiate.c \
    ../../../curl/http_ntlm.c \
    ../../../curl/http_proxy.c \
    ../../../curl/http2.c \
    ../../../curl/idn_win32.c \
    ../../../curl/if2ip.c \
    ../../../curl/inet_ntop.c \
    ../../../curl/inet_pton.c \
    ../../../curl/ldap.c \
    ../../../curl/llist.c \
    ../../../curl/md4.c \
    ../../../curl/memdebug.c \
    ../../../curl/multi.c \
    ../../../curl/netrc.c \
    ../../../curl/non-ascii.c \
    ../../../curl/nonblock.c \
    ../../../curl/nwlib.c \
    ../../../curl/nwos.c \
    ../../../curl/openldap.c \
    ../../../curl/parsedate.c \
    ../../../curl/pingpong.c \
    ../../../curl/pipeline.c \
    ../../../curl/progress.c \
    ../../../curl/rand.c \
    ../../../curl/rtsp.c \
    ../../../curl/select.c \
    ../../../curl/sendf.c \
    ../../../curl/share.c \
    ../../../curl/slist.c \
    ../../../curl/smb.c \
    ../../../curl/socks.c \
    ../../../curl/socks_gssapi.c \
    ../../../curl/socks_sspi.c \
    ../../../curl/speedcheck.c \
    ../../../curl/splay.c \
    ../../../curl/ssh.c \
    ../../../curl/strdup.c \
    ../../../curl/strcase.c \
    ../../../curl/strerror.c \
    ../../../curl/strtok.c \
    ../../../curl/strtoofft.c \
    ../../../curl/system_win32.c \
    ../../../curl/telnet.c \
    ../../../curl/tftp.c \
    ../../../curl/timeval.c \
    ../../../curl/transfer.c \
    ../../../curl/url.c \
    ../../../curl/warnless.c \
    ../../../curl/wildcard.c \
    ../../../curl/x509asn1.c \
    ../../../curl/vtls/axtls.c \
    ../../../curl/vtls/cyassl.c \
    ../../../curl/vtls/darwinssl.c \
    ../../../curl/vtls/gskit.c \
    ../../../curl/vtls/gtls.c \
    ../../../curl/vtls/mbedtls.c \
    ../../../curl/vtls/nss.c \
    ../../../curl/vtls/openssl.c \
    ../../../curl/vtls/polarssl.c \
    ../../../curl/vtls/polarssl_threadlock.c \
    ../../../curl/vtls/schannel.c \
    ../../../curl/vtls/vtls.c \
    ../../../curl/imap.c \
    ../../../curl/mprintf.c \
    ../../../curl/md5.c \
    ../../../curl/pop3.c \
    ../../../curl/smtp.c \
    ../../../curl/version.c \
    ../../../curl/axTLS/aes.c \
    ../../../curl/axTLS/asn1.c \
    ../../../curl/axTLS/bigint.c \
    ../../../curl/axTLS/crypto_misc.c \
    ../../../curl/axTLS/gen_cert.c \
    ../../../curl/axTLS/hmac.c \
    ../../../curl/axTLS/loader.c \
    ../../../curl/axTLS/md5.c \
    ../../../curl/axTLS/openssl.c \
    ../../../curl/axTLS/os_port.c \
    ../../../curl/axTLS/p12.c \
    ../../../curl/axTLS/rc4.c \
    ../../../curl/axTLS/rsa.c \
    ../../../curl/axTLS/sha1.c \
    ../../../curl/axTLS/sha256.c \
    ../../../curl/axTLS/sha384.c \
    ../../../curl/axTLS/sha512.c \
    ../../../curl/axTLS/tls1.c \
    ../../../curl/axTLS/tls1_clnt.c \
    ../../../curl/axTLS/tls1_svr.c \
    ../../../curl/axTLS/x509.c \
    ../../../curl/vauth/cleartext.c \
    ../../../curl/vauth/cram.c \
    ../../../curl/vauth/digest.c \
    ../../../curl/vauth/digest_sspi.c \
    ../../../curl/vauth/krb5_gssapi.c \
    ../../../curl/vauth/krb5_sspi.c \
    ../../../curl/vauth/ntlm.c \
    ../../../curl/vauth/ntlm_sspi.c \
    ../../../curl/vauth/oauth2.c \
    ../../../curl/vauth/spnego_gssapi.c \
    ../../../curl/vauth/spnego_sspi.c \
    ../../../curl/vauth/vauth.c

LOCAL_EXPORT_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM -lz -lOpenSLES 
LOCAL_CFLAGS += -DIDE_ANDROID -O3 -DUSE_AXTLS -DBUILDING_LIBCURL -fsigned-char
LOCAL_CPPFLAGS += -fexceptions -std=c++11
LOCAL_ARM_MODE := arm
LOCAL_SHORT_COMMANDS := false

# hack to avoid compilation output jumping out of the armeabi folder due to ../../../
ifeq ($(NDK_APP_OPTIM),debug)
TARGET_OBJS := $(TARGET_OUT)/objs-debug/objs2
else
TARGET_OBJS := $(TARGET_OUT)/objs/objs2
endif

include $(BUILD_STATIC_LIBRARY)
