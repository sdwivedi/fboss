# CMake to build libraries and binaries in fboss/agent/hw/bcm/tests

# In general, libraries and binaries in fboss/foo/bar are built by
# cmake/FooBar.cmake

add_executable(bcm_test
  fboss/agent/hw/bcm/tests/BcmTest.cpp
  fboss/agent/hw/bcm/tests/BcmTestUtils.cpp
  fboss/agent/hw/bcm/tests/BcmPortUtils.cpp
  fboss/agent/hw/bcm/tests/BcmColdBootStateTests.cpp
  fboss/agent/hw/bcm/tests/BcmSwitchEnsemble.cpp
  fboss/agent/hw/bcm/tests/HwSwitchEnsembleFactory.cpp
  fboss/agent/hw/bcm/tests/HwVlanUtils.cpp
  fboss/agent/hw/bcm/tests/HwTestMacUtils.cpp
  fboss/agent/hw/bcm/tests/HwTestMplsUtils.cpp
  fboss/agent/hw/bcm/tests/HwTestPacketTrapEntry.cpp
  fboss/agent/hw/bcm/tests/BcmAclCoppTests.cpp
  fboss/agent/hw/bcm/tests/BcmAclQualifierTests.cpp
  fboss/agent/hw/bcm/tests/BcmAclStatTests.cpp
  fboss/agent/hw/bcm/tests/BcmAclPriorityTests.cpp
  fboss/agent/hw/bcm/tests/BcmAclUnitTests.cpp
  fboss/agent/hw/bcm/tests/BcmAclUtils.cpp
  fboss/agent/hw/bcm/tests/BcmAddDelEcmpTests.cpp
  fboss/agent/hw/bcm/tests/BcmBstStatsMgrTest.cpp
  fboss/agent/hw/bcm/tests/BcmControlPlaneTests.cpp
  fboss/agent/hw/bcm/tests/BcmCosQueueManagerTest.cpp
  fboss/agent/hw/bcm/tests/BcmCosQueueManagerCounterTests.cpp
  fboss/agent/hw/bcm/tests/BcmDeathTests.cpp
  fboss/agent/hw/bcm/tests/BcmEcmpTests.cpp
  fboss/agent/hw/bcm/tests/BcmEcmpTrunkTests.cpp
  fboss/agent/hw/bcm/tests/BcmEmptyEcmpTests.cpp
  fboss/agent/hw/bcm/tests/BcmFieldProcessorTests.cpp
  fboss/agent/hw/bcm/tests/BcmHostKeyTests.cpp
  fboss/agent/hw/bcm/tests/BcmHostTests.cpp
  fboss/agent/hw/bcm/tests/BcmInterfaceTests.cpp
  fboss/agent/hw/bcm/tests/BcmLabelSwitchRouteTests.cpp
  fboss/agent/hw/bcm/tests/BcmLabeledEgressTest.cpp
  fboss/agent/hw/bcm/tests/BcmLabelSwitchActionTests.cpp
  fboss/agent/hw/bcm/tests/BcmLabelForwardingTests.cpp
  fboss/agent/hw/bcm/tests/BcmLinkStateDependentTests.cpp
  fboss/agent/hw/bcm/tests/BcmLinkStateToggler.cpp
  fboss/agent/hw/bcm/tests/BcmMatchActionsTests.cpp
  fboss/agent/hw/bcm/tests/BcmMirrorTests.cpp
  fboss/agent/hw/bcm/tests/BcmMplsTestUtils.cpp
  fboss/agent/hw/bcm/tests/BcmNeighborTests.cpp
  fboss/agent/hw/bcm/tests/BcmPortQueueManagerTests.cpp
  fboss/agent/hw/bcm/tests/BcmPortTests.cpp
  fboss/agent/hw/bcm/tests/BcmPortStressTests.cpp
  fboss/agent/hw/bcm/tests/BcmQosPolicyTests.cpp
  fboss/agent/hw/bcm/tests/BcmQosMapTests.cpp
  fboss/agent/hw/bcm/tests/BcmQueueStatCollectionTests.cpp
  fboss/agent/hw/bcm/tests/BcmRtag7Test.cpp
  fboss/agent/hw/bcm/tests/BcmRouteTests.cpp
  fboss/agent/hw/bcm/tests/BcmStateDeltaTests.cpp
  fboss/agent/hw/bcm/tests/BcmSwitchStateReplayTest.cpp
  fboss/agent/hw/bcm/tests/BcmTestRouteUtils.cpp
  fboss/agent/hw/bcm/tests/BcmTestStatUtils.cpp
  fboss/agent/hw/bcm/tests/BcmTrunkTests.cpp
  fboss/agent/hw/bcm/tests/BcmTrunkUtils.cpp
  fboss/agent/hw/bcm/tests/BcmUnitTests.cpp
  fboss/agent/hw/bcm/tests/QsetCmpTests.cpp
  fboss/agent/hw/bcm/tests/HwTestRouteUtils.cpp
  fboss/agent/hw/bcm/tests/oss/HwTestCoppUtils.cpp
  fboss/agent/hw/bcm/tests/oss/BcmSwitchEnsemble.cpp

  fboss/agent/hw/bcm/tests/dataplane_tests/BcmSflowTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmQueuePerHostTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmTestQueuePerHostUtils.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmMultiAqmProfileTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmOlympicQoSTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmAclCounterTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmTest2QueueUtils.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmConfigVerifyQosTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/Bcm2QueueToOlympicQoSTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmQosUtils.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmMirroringTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmConfigSetupTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmPortBandwidthTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmDscpQueueMappingTests.cpp
  fboss/agent/hw/bcm/tests/dataplane_tests/BcmEcnTests.cpp
)

target_compile_definitions(bcm_test
  PUBLIC
    ${LIBGMOCK_DEFINES}
)

target_include_directories(bcm_test
  PUBLIC
    ${LIBGMOCK_INCLUDE_DIR}
)

target_link_libraries(bcm_test
  handler
  setup_thrift
  bcm
  bcm_mpls_utils
  config
  counter_utils
  # --whole-archive is needed for gtest to find these tests
  -Wl,--whole-archive
  hw_switch_test
  hw_test_main
  -Wl,--no-whole-archive
  product_info
  bcm_test_platforms
  ecmp_helper
  route_scale_gen
  trunk_utils
  sflow_cpp2
  packettrace_cpp2
  Folly::folly
  ${GTEST}
  ${LIBGMOCK_LIBRARIES}
)
