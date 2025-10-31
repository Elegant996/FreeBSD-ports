--- iavf_lib.c.orig	2024-09-06 05:36:15 UTC
+++ iavf_lib.c
@@ -39,7 +39,7 @@
  * hardware initialization and control functions, as well as sysctl handlers
  * for the sysctls which are shared between the legacy and iflib drivers.
  */
-#include "iavf.h"
+#include "iavf_iflib.h"
 #include "iavf_vc_common.h"
 
 static void iavf_init_hw(struct iavf_hw *hw, device_t dev);
@@ -234,7 +234,7 @@
 	int error = 0, ret_error = 0, asq_retries = 0;
 	bool send_api_ver_retried = 0;
 
-	/* Need to set these AQ paramters before initializing AQ */
+	/* Need to set these AQ parameters before initializing AQ */
 	hw->aq.num_arq_entries = IAVF_AQ_LEN;
 	hw->aq.num_asq_entries = IAVF_AQ_LEN;
 	hw->aq.arq_buf_size = IAVF_AQ_BUF_SZ;
@@ -513,7 +513,7 @@
 
 	for (int i = 0; i < sc->vf_res->num_vsis; i++) {
 		/* XXX: We only use the first VSI we find */
-		if (sc->vf_res->vsi_res[i].vsi_type == IAVF_VSI_SRIOV)
+		if (sc->vf_res->vsi_res[i].vsi_type == VIRTCHNL_VSI_SRIOV)
 			sc->vsi_res = &sc->vf_res->vsi_res[i];
 	}
 	if (!sc->vsi_res) {
@@ -758,11 +758,7 @@
 {
 	struct iavf_hw		*hw = &sc->hw;
 	struct iavf_vsi		*vsi = &sc->vsi;
-#ifdef IAVF_NO_IFLIB
-	struct iavf_queue	*que = vsi->queues;
-#else
 	struct iavf_tx_queue	*que = vsi->tx_queues;
-#endif
 
 	vsi->tx_itr_setting = sc->tx_itr;
 
@@ -787,11 +783,7 @@
 {
 	struct iavf_hw		*hw = &sc->hw;
 	struct iavf_vsi		*vsi = &sc->vsi;
-#ifdef IAVF_NO_IFLIB
-	struct iavf_queue	*que = vsi->queues;
-#else
 	struct iavf_rx_queue	*que = vsi->rx_queues;
-#endif
 
 	vsi->rx_itr_setting = sc->rx_itr;
 
@@ -977,7 +969,7 @@
  * @returns ENODEV because changing the media and speed is not supported.
  */
 int
-iavf_media_change_common(struct ifnet *ifp)
+iavf_media_change_common(if_t ifp)
 {
 	if_printf(ifp, "Changing speed is not supported\n");
 
@@ -993,7 +985,7 @@
  * reports speed to us over the virtchnl interface.
  */
 void
-iavf_set_initial_baudrate(struct ifnet *ifp)
+iavf_set_initial_baudrate(if_t ifp)
 {
 	if_setbaudrate(ifp, IF_Gbps(40));
 }
@@ -1116,19 +1108,19 @@
 #ifdef RSS
 	rss_hash_config = rss_gethashconfig();
 	if (rss_hash_config & RSS_HASHTYPE_RSS_IPV4)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV4_OTHER);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV4_OTHER);
 	if (rss_hash_config & RSS_HASHTYPE_RSS_TCP_IPV4)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV4_TCP);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV4_TCP);
 	if (rss_hash_config & RSS_HASHTYPE_RSS_UDP_IPV4)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV4_UDP);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV4_UDP);
 	if (rss_hash_config & RSS_HASHTYPE_RSS_IPV6)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV6_OTHER);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV6_OTHER);
 	if (rss_hash_config & RSS_HASHTYPE_RSS_IPV6_EX)
-		set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV6);
+		set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_FRAG_IPV6);
 	if (rss_hash_config & RSS_HASHTYPE_RSS_TCP_IPV6)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV6_TCP);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV6_TCP);
         if (rss_hash_config & RSS_HASHTYPE_RSS_UDP_IPV6)
-                set_hena |= ((u64)1 << VIRTCHNL_HASH_FILTER_IPV6_UDP);
+                set_hena |= ((u64)1 << IAVF_FILTER_PCTYPE_NONF_IPV6_UDP);
 #else
 	set_hena = IAVF_DEFAULT_RSS_HENA_XL710;
 #endif
@@ -1215,7 +1207,7 @@
 int
 iavf_config_promisc(struct iavf_sc *sc, int flags)
 {
-	struct ifnet *ifp = sc->vsi.ifp;
+	if_t ifp = sc->vsi.ifp;
 
 	sc->promisc_flags = 0;
 
@@ -1474,31 +1466,6 @@
 }
 
 /**
- * iavf_update_msix_devinfo - Fix MSIX values for pci_msix_count()
- * @dev: pointer to kernel device
- *
- * Fix cached MSI-X control register information. This is a workaround
- * for an issue where VFs spawned in non-passthrough mode on FreeBSD
- * will have their PCI information cached before the PF driver
- * finishes updating their PCI information.
- *
- * @pre Must be called before pci_msix_count()
- */
-void
-iavf_update_msix_devinfo(device_t dev)
-{
-	struct pci_devinfo *dinfo;
-	u32 msix_ctrl;
-	u8 msix_location;
-
-	dinfo = (struct pci_devinfo *)device_get_ivars(dev);
-	msix_location = dinfo->cfg.msix.msix_location;
-	msix_ctrl = pci_read_config(dev, msix_location + PCIR_MSIX_CTRL, 2);
-	dinfo->cfg.msix.msix_ctrl = msix_ctrl;
-	dinfo->cfg.msix.msix_msgnum = (msix_ctrl & PCIM_MSIXCTRL_TABLE_SIZE) + 1;
-}
-
-/**
  * iavf_disable_queues_with_retries - Send PF multiple DISABLE_QUEUES messages
  * @sc: device softc
  *
