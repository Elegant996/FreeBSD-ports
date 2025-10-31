--- iavf_lib.h.orig	2024-09-06 05:36:15 UTC
+++ iavf_lib.h
@@ -175,28 +175,28 @@
 #define IAVF_FLAGS_KEEP_TSO6	(1 << 1)
 
 #define IAVF_DEFAULT_RSS_HENA_BASE (\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV4_UDP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV4_TCP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV4_SCTP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV4_OTHER) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_FRAG_IPV4) |		\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV6_UDP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV6_TCP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV6_SCTP) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV6_OTHER) |	\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_FRAG_IPV6))
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV4_UDP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV4_TCP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV4_SCTP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV4_OTHER) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_FRAG_IPV4) |		\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV6_UDP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV6_TCP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV6_SCTP) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV6_OTHER) |	\
+	BIT_ULL(IAVF_FILTER_PCTYPE_FRAG_IPV6))
 
 #define IAVF_DEFAULT_ADV_RSS_HENA (\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_UNICAST_IPV4_UDP) | \
-	BIT_ULL(VIRTCHNL_HASH_FILTER_MULTICAST_IPV4_UDP) | \
-	BIT_ULL(VIRTCHNL_HASH_FILTER_UNICAST_IPV6_UDP) | \
-	BIT_ULL(VIRTCHNL_HASH_FILTER_MULTICAST_IPV6_UDP) | \
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV4_TCP_SYN_NO_ACK) | \
-	BIT_ULL(VIRTCHNL_HASH_FILTER_IPV6_TCP_SYN_NO_ACK))
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_UNICAST_IPV4_UDP) | \
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_MULTICAST_IPV4_UDP) | \
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_UNICAST_IPV6_UDP) | \
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_MULTICAST_IPV6_UDP) | \
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV4_TCP_SYN_NO_ACK) | \
+	BIT_ULL(IAVF_FILTER_PCTYPE_NONF_IPV6_TCP_SYN_NO_ACK))
 
 #define IAVF_DEFAULT_RSS_HENA_XL710 (\
 	IAVF_DEFAULT_RSS_HENA_BASE |			\
-	BIT_ULL(VIRTCHNL_HASH_FILTER_L2_PAYLOAD))
+	BIT_ULL(IAVF_FILTER_PCTYPE_L2_PAYLOAD))
 
 #define IAVF_DEFAULT_RSS_HENA_X722 (\
 	IAVF_DEFAULT_RSS_HENA_XL710 |			\
@@ -228,13 +228,8 @@
 #define IAVF_CAP_ADV_LINK_SPEED(_sc) \
     ((_sc)->vf_res->vf_cap_flags & VIRTCHNL_VF_CAP_ADV_LINK_SPEED)
 
-#ifdef IAVF_NO_IFLIB
-#define IAVF_NRXQS(_vsi) ((_vsi)->num_queues)
-#define IAVF_NTXQS(_vsi) ((_vsi)->num_queues)
-#else
 #define IAVF_NRXQS(_vsi) ((_vsi)->num_rx_queues)
 #define IAVF_NTXQS(_vsi) ((_vsi)->num_tx_queues)
-#endif
 
 /**
  * printf %b flag args
@@ -464,8 +459,8 @@
     struct sysctl_oid_list *child, struct iavf_eth_stats *eth_stats);
 void iavf_media_status_common(struct iavf_sc *sc,
     struct ifmediareq *ifmr);
-int iavf_media_change_common(struct ifnet *ifp);
-void iavf_set_initial_baudrate(struct ifnet *ifp);
+int iavf_media_change_common(if_t ifp);
+void iavf_set_initial_baudrate(if_t ifp);
 u64 iavf_max_vc_speed_to_value(u8 link_speeds);
 void iavf_config_rss_reg(struct iavf_sc *sc);
 void iavf_config_rss_pf(struct iavf_sc *sc);
@@ -481,7 +476,6 @@
 u64 iavf_baudrate_from_link_speed(struct iavf_sc *sc);
 void iavf_add_vlan_filter(struct iavf_sc *sc, u16 vtag);
 int iavf_mark_del_vlan_filter(struct iavf_sc *sc, u16 vtag);
-void iavf_update_msix_devinfo(device_t dev);
 void iavf_disable_queues_with_retries(struct iavf_sc *);
 
 int iavf_sysctl_current_speed(SYSCTL_HANDLER_ARGS);
