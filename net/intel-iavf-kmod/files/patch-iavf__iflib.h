--- iavf_iflib.h.orig	2025-11-01 01:18:23 UTC
+++ iavf_iflib.h
@@ -41,10 +41,6 @@
 #ifndef _IAVF_IFLIB_H_
 #define _IAVF_IFLIB_H_
 
-#ifdef IAVF_NO_IFLIB
-#error "Do not include iavf_iflib.h in a legacy build"
-#endif
-
 #include "iavf_opts.h"
 
 #include <sys/param.h>
@@ -106,7 +102,6 @@
 #include "ifdi_if.h"
 
 #include "iavf_lib.h"
-#include "freebsd_compat_iflib.h"
 
 #define IAVF_CSUM_TCP \
 	(CSUM_IP_TCP|CSUM_IP_TSO|CSUM_IP6_TSO|CSUM_IP6_TCP)
@@ -128,6 +123,7 @@
 #define iavf_sc_from_ctx(_ctx) \
     ((struct iavf_sc *)iflib_get_softc(_ctx))
 
+/* Use the correct assert function for each lock type */
 #define IAVF_VC_LOCK(_sc)                mtx_lock(&(_sc)->vc_mtx)
 #define IAVF_VC_UNLOCK(_sc)              mtx_unlock(&(_sc)->vc_mtx)
 #define IAVF_VC_LOCK_DESTROY(_sc)        mtx_destroy(&(_sc)->vc_mtx)
@@ -240,7 +236,7 @@
 struct iavf_vsi {
 	if_ctx_t		ctx;
 	if_softc_ctx_t		shared;
-	struct ifnet		*ifp;
+	if_t			ifp;
 	struct iavf_sc		*back;
 	device_t		dev;
 	struct iavf_hw		*hw;
