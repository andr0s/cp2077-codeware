#pragma once

namespace Raw::Puppet
{
#if defined(__APPLE__)
// macOS: clang packs the field into the base class tail padding (GetRecordID native 0x10378A87C reads +0x25C).
using RecordID = Core::OffsetPtr<0x25C, Red::TweakDBID>;
#else
using RecordID = Core::OffsetPtr<0x260, Red::TweakDBID>;
#endif
}
