# Codeware for macOS: port notes

Codeware 1.20.5 (psiberx/cp2077-codeware at a88e8aa5) ported to Cyberpunk 2077 2.3.1 (Steam, macOS, Apple silicon)
for the jackmaxwil RED4ext macOS loader and RED4ext.SDK-macos (f657158c). Branch `macos`. macOS code is behind
`#if defined(__APPLE__)` (or in macOS-only files), so the Windows build is unchanged and the branch stays rebaseable.

Status: source-complete and syntax-checked; **not built, not run, not tested in game** (nothing was executed).

## What works (by priority)

1. **Plugin load and natives.** RED4ext entry point, CMake build of `Codeware.dylib`, RTTI registration of every
   Codeware type and native (none removed; natives that cannot work on macOS return defaults and log once). Script
   imports of eight Windows-only game types are left out of the install (see "Compiled out").
2. **Reflection, scripting helpers, lifecycle, callbacks.** Reflection API; script validator hooks (Validate,
   ValidateProperty, CompareType, with Codeware's type-name rules as a fallback because CompareTypeName is inlined);
   `new`/cast opcode handlers; `GetGameInstance`; OnConstruct; ScriptableService container and lifecycle
   (state persistence once the ObjectSerializer addresses are merged); CallbackSystem with game session events
   (Session/BeforeStart, Start, Ready, BeforeEnd, End), entity lifecycle events (Initialize, Reassemble, Assemble,
   Attach, Detach, Uninitialize, Extract), Component/Toggle, Vehicle/ToggleAuxLights, Input/Key and Input/Axis,
   InkWidget/Spawn.
3. **Entity spawning.** DynamicEntitySystem (stub spawning, population registration, persistent/transient IDs, tags,
   listeners, save/restore) and StaticEntitySystem (entity spawner, attach/detach, tags) with the macOS virtual call
   conventions; EntityEx helpers (SetWorldTransform, ApplyMorphTarget, components), record ID resolution for NPCs and
   vehicles.
4. **UI and localization.** inkSystem helpers (layers, world widgets, clipboard through NSPasteboard), widget building
   (reparent/add child layer attachment, logic controllers), widget spawning with controller injection, key input
   events (macOS US-ANSI character mapping from CGEventSource modifier state), localization providers.
5. **Everything else.** Resource depot and references, mesh appearance helpers, physics trace hit objects, weather,
   vehicle garage, quest node execution, open world activity tracking, journal and mappin helpers, community and
   population spawner wrappers, persistency service, world node instance accessors, streaming sector node buffers,
   the streaming world getter, wardrobe ForgetItemID, string/hash/bit utilities.

## Compiled out or stubbed on macOS (each logs one line when used)

| feature | reason |
|---|---|
| Resource/Load and Resource/PostLoad callback events | the resource serializer's job dispatchers are inlined on macOS (re_res.md §3) |
| IScriptable/ISerializable `Clone` | no generic deep clone function exists on macOS (re_res.md §2); returns null |
| `CommunityEntryWrapper.GetRestoredEntityIDs` | that array holds 0x28-byte restore records on macOS, not entity IDs; returns empty |
| `WorkspotGameSystem.GetWorkspotInstance` | lookup inlined; map layout/hash and instance property offsets differ or are unverified; returns null |
| StaticEntitySystem tick (prunes disposed static entities) | needs the SDK hash UpdateRegistrar_RegisterGroupUpdate, which is unverified in the database |
| Script imports `FSR30CustomData`, `gsmMenuState_LoadSession`, `inputInputSystemEditor`, `inputInputSystemWin32Base`, `inputInputSystemWin32Game`, `oauthAuthenticationToken`, `servicesGameServicesWin`, `toolsJiraService` | these classes do not exist in the macOS game (no RTTI name strings; macOS has e.g. `inputInputSystemMac`); the script validator fails the whole load on "Missing native class". Not referenced by any other script; excluded by the CMake install rules |
| Any address not merged yet | resolves to 0: hooks are refused (feature off, logged) and direct calls are skipped and return a default value (logged once per signature) |

Behaviour differences: Component/Toggle does not see toggles called from scripts (the native inlines the toggle);
community spawner requests apply on the next frame (Update does not flush them on macOS); the resource path registry
(KnownHashes) is not shared with ArchiveXL through RTTI storage, so each plugin keeps its own map and hook;
`Entity/Initialize` fires before Entity::Initialize (RequestComponents is inlined), with the entity and record IDs set.

## Notable macOS changes

- **Fail-closed addresses**: `src/Red/Addresses/Library.hpp` wraps unavailable hashes in
  `RED_ADDRESS_MACOS_UNAVAILABLE` (0 on macOS). `lib/Core/Raw.hpp`: a call through an unresolved `RawFunc` lands in a
  stub that logs once and returns a default value instead of jumping to null. Hook installation failures log
  instead of throwing (`HookResult::OrThrow`), because an exception would escape the plugin's Main.
- **ABI**: sret/void/argument differences and macOS vtable slots are listed in `ADDRESSES.md`; game-system virtuals
  with macOS conventions are in `src/Red/GameSystemsMacOS.hpp`. Interface slots verified by declaration order:
  IPopulationSystem (FindEntity +0x1B8 needs the macOS call; IsSpawning +0x1D8, RegisterEntity +0x1F8,
  RemoveEntity +0x200 match the SDK), IEntityStubSystem (DeleteStub matches), IPersistencySystem (ResetPersistentState
  +0x1D8 and RemoveDynamicEntityState +0x1F0 match), IEntitySpawnerEventsBroadcaster (match).
- **Library ports** taken from the jackmaxwil ArchiveXL macOS port (runtime/host image, platform layer, RED4ext
  provider with hooking and address library, spdlog file logging); `Core::Map` keeps the tsl-style `key()/value()`
  iterator API on `std::unordered_map`; `std::hash` for 64-bit game keys; plugin enums built on the game's CEnum
  vtable (the SDK CEnum overrides use unverified hashes).
- **Layouts read directly** (Codeware's `OffsetPtr`s): checked on macOS in `_work/codeware/re_world.md` and the
  other reports. worldINodeInstance (0x30/0x50/0x60/0x70/0x80/0x88), the setup info and sector node buffer (+0x40),
  the streaming world handle (+0x270), the wardrobe item store (+0x48, lock +0xF4) and the entity fields are the same
  as on Windows; the ones that differ are listed in `ADDRESSES.md`. worldBendedMeshNodeInstance's mesh handle is at
  +0xC0 on macOS (Windows +0xA0); Codeware declares it but does not use it, so it is left unchanged.
- **Entry point**: `src/main.cpp` tolerates a repeated Load/Unload from the macOS loader; `Application` registers the
  RED4ext hooking provider instead of MinHook and resolves the game root three levels above the executable.

## Addresses

107 hashes: 18 verified in the database, 61 proposed with evidence (56 high, 5 medium; 10 of them correct wrong
unverified database entries and must be merged before RED4ext will load Codeware), 28 disabled. Details, evidence and
the full table: `ADDRESSES.md`; mergeable entries: `addresses_proposed.json`.

## Build (not run)

Requirements: Xcode 16.3+ or its Command Line Tools (AppleClang 17, macOS SDK 15+), CMake 3.20+. The Command Line
Tools installed on this Mac are Apple clang 12 with the macOS 11.0 SDK, which cannot compile this code (C++20
`std::format`, concepts), so a newer toolchain is needed; nothing was installed for this port (installing it is your
call). The syntax check used Homebrew LLVM 21 headers only; do not link the plugin against Homebrew's libc++.

```sh
cd <cp2077-mac-port>/ports/Codeware
# SDK: either the submodule (jackmaxwil/RED4ext.SDK-macos @ f657158c) ...
git submodule update --init vendor/RED4ext.SDK
# ... or an existing checkout via -DRED4EXT_SDK_DIR (shown below)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DRED4EXT_SDK_DIR=<cp2077-mac-port>/src/jackmaxwil_RED4ext.SDK-macos
cmake --build build -j8
```

Configure downloads spdlog 1.16.0 from GitHub (FetchContent, pinned SHA256); nothing else is fetched. To configure
offline, pass `-DFETCHCONTENT_SOURCE_DIR_SPDLOG=<spdlog 1.16 source tree>` (the spdlog 1.12 in the RED4ext checkout is
too old for current clang). Release is the default build type (a Debug build keeps SDK code paths that reference
unverified hashes).

Before installing, check the dylib against the database RED4ext uses (after merging the proposed addresses):

```sh
GAME="$HOME/Library/Application Support/Steam/steamapps/common/Cyberpunk 2077"
"$GAME/red4ext/bin/red4ext_plugin_check" "$GAME/red4ext/bin/x64/cyberpunk2077_addresses.json" build/Codeware.dylib
# expected: OK build/Codeware.dylib
```

## Install layout

`cmake --install build --prefix "$GAME"` produces:

```
<Cyberpunk 2077>/red4ext/plugins/Codeware/
  Codeware.dylib
  Scripts/...            redscript sources (scripts/ minus the eight Windows-only imports)
  Data/KnownHashes.txt
  LICENSE, THIRD_PARTY_LICENSES
```

At runtime Codeware creates `Codeware-<timestamp>.log` and a `Codeware.log` symlink next to the dylib, and
`Persistent/ScriptableServiceContainer.dat`. The RED4ext launcher stages `Scripts/` into
`r6/scripts/zz_red4ext_plugins/Codeware` for the script compiler only when the plugin passes the address check.

## What to verify in game

1. `Codeware.log`: "Codeware 1.20.5 is initializing..." / "is initialized"; only the expected `[macOS] ... not
   available` lines; no hook failures for the scripting, entity, ink or localization hooks.
2. Scripts: the launcher's script compile and the game's validation report no "Missing native class/property" and
   no "Validation failed"; `Codeware.Require("1.20.5")` works from a mod.
3. Lifecycle: a ScriptableService gets OnLoad, OnInitialize, OnUninitialize; Session/Start and Session/Ready fire once
   after loading a save (Session/Ready comes from the medium-confidence PlayerSystem_OnPlayerSpawned).
4. Entity events with filters: Entity/Initialize for NPCs filtered by record ID (`EntityTarget.RecordID`), Attach,
   Detach, Uninitialize, Assemble, Reassemble.
5. DynamicEntitySystem: CreateEntity from a record and from a template spawns in view; GetEntity, IsSpawning, tags,
   DeleteEntity; persistent entities survive save/load; transient ones do not leak.
6. UI: script-built widgets attached to a layer, custom logic controllers initialize, `item:Controller` spawning,
   inkKeyInputEvent characters with Shift and Caps Lock, clipboard text round trip.
7. Localization provider texts; weather Set/Reset; quest ExecuteNode; journal/mappin helpers; ScriptableService state
   persists across sessions (needs the ObjectSerializer addresses).
8. Save, load, and quit without crashes.

## Security-relevant notes

- No network access at runtime; no subprocesses (no `popen`, `system`, `fork`/`exec`); no files outside the game
  folder (nothing in `/tmp`). The only download is spdlog at CMake configure time (pinned SHA256).
- Files written: the plugin folder only (logs; old timestamped Codeware logs there are deleted;
  `Persistent/ScriptableServiceContainer.dat`). Upstream's migration deletes `<game>/r6/scripts/Codeware` (a legacy
  Codeware script folder) at startup.
- Files read: `Data/KnownHashes.txt`; game resources only through the game.
- Clipboard: the `inkSystem` natives GetClipboardText/SetClipboardText give any redscript mod read/write access to the
  system clipboard (NSPasteboard via the clip library), as on Windows.
- `setlocale(LC_ALL, "en_US.UTF-8")` is called process-wide at load (upstream behaviour).
- Hooks only through RED4ext's hooking API; the RTTI system's shared storage is not touched at load on macOS.
- The Enrique53xD port's `/tmp` debug log and `pbpaste`/`pbcopy` bridge were not carried over.

## Checks done

- `-fsyntax-only` compile of all 44 translation units with Homebrew clang 21 + the macOS SDK headers
  (`-std=c++20 -mmacosx-version-min=14.0 -DRED4EXT_PLATFORM_MACOS`, spdlog headers only): all pass.
- Script-declared native types checked against the macOS binary's RTTI name strings (6080 types; the eight above are
  missing). Native function names on game types were all found; property names cannot be checked this way.
- RE reports spot-checked against the binary (call sites, vtable slots, field loads).
