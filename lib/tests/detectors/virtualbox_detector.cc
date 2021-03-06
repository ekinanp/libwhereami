#include <catch.hpp>
#include <internal/sources/dmi_source.hpp>
#include <internal/detectors/virtualbox_detector.hpp>
#include "../fixtures/dmi_fixtures.hpp"
#include "../fixtures/cpuid_fixtures.hpp"
#include "../fixtures/system_profiler_fixtures.hpp"

using namespace std;
using namespace whereami::detectors;
using namespace whereami::sources;
using namespace whereami::testing::dmi;
using namespace whereami::testing::cpuid;
using namespace whereami::testing::system_profiler;

SCENARIO("Using the VirtualBox detector") {
    WHEN("running on a Linux VirtualBox guest with root") {
        leaf_register_map values {
            {VENDOR_LEAF, {
                {0, register_fixtures::VENDOR_KVMKVMKVM}}},
            {HYPERVISOR_PRESENT_LEAF, {
                {0, register_fixtures::HYPERVISOR_PRESENT}}}};
        cpuid_fixture cpuid_source {values};
        dmi_fixture_values dmi_source({
            "0x30000",
            "VirtualBox",
            "VirtualBox",
            "Oracle Corporation",
            "innotek GmbH",
            "VirtualBox",
            {
                "vboxVer_5.1.22",
                "vboxRev_115126",
            }});
        system_profiler_fixture system_profiler_source({});
        auto res = virtualbox(cpuid_source, dmi_source, system_profiler_source);
        THEN("the result should be valid") {
            REQUIRE(res.valid());
        }
        AND_THEN("the result should contain privileged VirtualBox metadata") {
            REQUIRE(res.get<string>("version") == "5.1.22");
            REQUIRE(res.get<string>("revision") == "115126");
        }
    }

    WHEN("running on a Linux VirtualBox guest without root") {
        leaf_register_map values {
            {VENDOR_LEAF, {
                {0, register_fixtures::VENDOR_KVMKVMKVM}}},
            {HYPERVISOR_PRESENT_LEAF, {
                {0, register_fixtures::HYPERVISOR_PRESENT}}}};
        cpuid_fixture cpuid_source {values};
        dmi_fixture_values dmi_source({
            "",
            "VirtualBox",
            "VirtualBox",
            "Oracle Corporation",
            "innotek GmbH",
            "VirtualBox",
            {}});
        system_profiler_fixture system_profiler_source({});
        auto const& res = virtualbox(cpuid_source, dmi_source, system_profiler_source);
        THEN("the result should be valid") {
            REQUIRE(res.valid());
        }
        AND_THEN("the result should not contain privileged VirtualBox metadata") {
            REQUIRE(res.get<string>("version") == "");
            REQUIRE(res.get<string>("revision") == "");
        }
    }

    WHEN("running on a Windows VirtualBox guest") {
        leaf_register_map values {
            {VENDOR_LEAF, {
                {0, register_fixtures::VENDOR_VBoxVBoxVBox}}},
            {HYPERVISOR_PRESENT_LEAF, {
                {0, register_fixtures::HYPERVISOR_PRESENT}}}};
        cpuid_fixture cpuid_source {values};
        dmi_fixture_empty dmi_source;
        system_profiler_fixture system_profiler_source({});
        THEN("the result should be valid") {
            auto res = virtualbox(cpuid_source, dmi_source, system_profiler_source);
            REQUIRE(res.valid());
        }
    }

    WHEN("running on a MacOS VirtualBox guest") {
        leaf_register_map values {
            {VENDOR_LEAF, {
                {0, register_fixtures::VENDOR_NONE}}},
            {HYPERVISOR_PRESENT_LEAF, {
                {0, register_fixtures::HYPERVISOR_ABSENT}}}};
        cpuid_fixture cpuid_source {values};
        dmi_fixture_empty dmi_source;
        system_profiler_fixture system_profiler_source(system_profiler_fixture::SYSTEM_PROFILER_VIRTUALBOX);
        THEN("the reuslt should be valid") {
            auto res = virtualbox(cpuid_source, dmi_source, system_profiler_source);
            REQUIRE(res.valid());
        }
    }

    WHEN("running outside of VirtualBox") {
        leaf_register_map values {
            {VENDOR_LEAF, {
                {0, register_fixtures::VENDOR_KVMKVMKVM}}},
            {HYPERVISOR_PRESENT_LEAF, {
                {0, register_fixtures::HYPERVISOR_PRESENT}}}};
        cpuid_fixture cpuid_source {values};
        dmi_fixture_values dmi_source({
            "Other",
            "Other",
            "Other",
            "Other",
            "Other",
            "Other",
            {}});
        system_profiler_fixture system_profiler_source({});
        THEN("the result should not be valid") {
            auto res = virtualbox(cpuid_source, dmi_source, system_profiler_source);
            REQUIRE_FALSE(res.valid());
        }
    }
}
