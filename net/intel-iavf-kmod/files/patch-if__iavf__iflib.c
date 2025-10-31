--- if_iavf_iflib.c.orig	2024-09-06 05:36:15 UTC
+++ if_iavf_iflib.c
@@ -76,6 +76,7 @@
 static uint64_t	 iavf_if_get_counter(if_ctx_t ctx, ift_counter cnt);
 static void	 iavf_if_init(if_ctx_t ctx);
 static void	 iavf_if_stop(if_ctx_t ctx);
+static bool	 iavf_if_needs_restart(if_ctx_t, enum iflib_restart_event);
 
 static int	iavf_allocate_pci_resources(struct iavf_sc *);
 static void	iavf_free_pci_resources(struct iavf_sc *);
@@ -126,11 +127,7 @@
 	"iavf", iavf_methods, sizeof(struct iavf_sc),
 };
 
-/**
- * Use a compatibility macro to properly declare the driver on both 14.0+ and
- * previous kernels.
- */
-COMPAT_DRIVER_MODULE(iavf, pci, iavf_driver, 0);
+DRIVER_MODULE(iavf, pci, iavf_driver, 0, 0);
 MODULE_VERSION(iavf, 1);
 
 MODULE_DEPEND(iavf, pci, 1, 1, 1);
@@ -175,6 +172,7 @@
 	DEVMETHOD(ifdi_vlan_register, iavf_if_vlan_register),
 	DEVMETHOD(ifdi_vlan_unregister, iavf_if_vlan_unregister),
 	DEVMETHOD(ifdi_get_counter, iavf_if_get_counter),
+	DEVMETHOD(ifdi_needs_restart, iavf_if_needs_restart),
 	DEVMETHOD_END
 };
 
@@ -383,9 +381,6 @@
 	scctx->isc_capabilities = scctx->isc_capenable = IAVF_CAPS;
 	scctx->isc_tx_csum_flags = CSUM_OFFLOAD;
 
-	/* Update OS cache of MSIX control register values */
-	iavf_update_msix_devinfo(dev);
-
 	return (0);
 
 err_vc_tq:
@@ -467,13 +462,13 @@
 static int
 iavf_if_attach_post(if_ctx_t ctx)
 {
+#ifdef IXL_DEBUG
+	device_t dev = iflib_get_dev(ctx);
+#endif
 	struct iavf_sc	*sc;
 	struct iavf_hw	*hw;
 	struct iavf_vsi *vsi;
 	int error = 0;
-#ifdef IAVF_DEBUG
-	device_t dev = iflib_get_dev(ctx);
-#endif
 
 	INIT_DBG_DEV(dev, "begin");
 
@@ -766,7 +761,7 @@
 	/* Make sure queues are disabled */
 	iavf_disable_queues_with_retries(sc);
 
-	bcopy(IF_LLADDR(ifp), tmpaddr, ETHER_ADDR_LEN);
+	bcopy(if_getlladdr(ifp), tmpaddr, ETHER_ADDR_LEN);
 	if (!cmp_etheraddr(hw->mac.addr, tmpaddr) &&
 	    (iavf_validate_mac_addr(tmpaddr) == IAVF_SUCCESS)) {
 		error = iavf_del_mac_filter(sc, hw->mac.addr);
@@ -851,9 +846,7 @@
 
 		snprintf(buf, sizeof(buf), "rxq%d", i);
 		err = iflib_irq_alloc_generic(ctx, &rx_que->que_irq, rid,
-		    IFLIB_INTR_RX, iavf_msix_que, rx_que, rx_que->rxr.me, buf);
-		/* XXX: Does the driver work as expected if there are fewer num_rx_queues than
-		 * what's expected in the iflib context? */
+		    IFLIB_INTR_RXTX, iavf_msix_que, rx_que, rx_que->rxr.me, buf);
 		if (err) {
 			device_printf(iflib_get_dev(ctx),
 			    "Failed to allocate queue RX int vector %d, err: %d\n", i, err);
@@ -871,9 +864,6 @@
 		    &vsi->rx_queues[i % vsi->shared->isc_nrxqsets].que_irq,
 		    IFLIB_INTR_TX, tx_que, tx_que->txr.me, buf);
 
-		/* TODO: Maybe call a strategy function for this to figure out which
-		* interrupts to map Tx queues to. I don't know if there's an immediately
-		* better way than this other than a user-supplied map, though. */
 		tx_que->msix = (i % vsi->shared->isc_nrxqsets) + 1;
 	}
 
@@ -1508,6 +1498,25 @@
 	}
 }
 
+/* iavf_if_needs_restart - Tell iflib when the driver needs to be reinitialized
+ * @ctx: iflib context
+ * @event: event code to check
+ *
+ * Defaults to returning false for unknown events.
+ *
+ * @returns true if iflib needs to reinit the interface
+ */
+static bool
+iavf_if_needs_restart(if_ctx_t ctx __unused, enum iflib_restart_event event)
+{
+	switch (event) {
+	case IFLIB_RESTART_VLAN_CONFIG:
+		return (true);
+	default:
+		return (false);
+	}
+}
+
 /**
  * iavf_free_pci_resources - Free PCI resources
  * @sc: device softc
@@ -1550,12 +1559,12 @@
 {
 	struct iavf_vsi *vsi = &sc->vsi;
 	if_ctx_t ctx = vsi->ctx;
-	struct ifnet *ifp = iflib_get_ifp(ctx);
+	if_t ifp = iflib_get_ifp(ctx);
 
 	iavf_dbg_init(sc, "begin\n");
 
 	vsi->shared->isc_max_frame_size =
-	    ifp->if_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN
+	    if_getmtu(ifp) + ETHER_HDR_LEN + ETHER_CRC_LEN
 	    + ETHER_VLAN_ENCAP_LEN;
 
 	iavf_set_initial_baudrate(ifp);
