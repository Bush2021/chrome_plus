# Key Mapping Feature Implementation

## Overview
A new key mapping feature has been added to Chrome++ that allows users to remap keyboard keys to different key combinations.

## Files Created
- `src/keymapping.h` - Header file with function declarations
- `src/keymapping.cc` - Implementation file with key mapping logic

## Files Modified
- `src/chrome++.cc` - Integrated key mapping initialization and cleanup

## How It Works
The feature uses a low-level keyboard hook (WH_KEYBOARD_LL) to intercept key presses and remap them to different key combinations according to user configuration.

## Configuration Format
Add a `[keymapping]` section to `chrome++.ini`:

```ini
[keymapping]
; Format: SourceKey=TargetKeyCombination
; Multiple mappings separated by semicolons (;)
; Example: F2=Ctrl+PageUp maps F2 key to Ctrl+PageUp
mappings=F2=Ctrl+PageUp;F3=Ctrl+PageDown
```

## Supported Keys
- Modifier keys: Ctrl, Alt, Shift
- Function keys: F1-F24
- Navigation keys: Left, Right, Up, Down, Home, End, PageUp, PageDown
- Special keys: Esc, Tab, Backspace, Enter, Space, Insert, Delete
- Alphanumeric: A-Z, 0-9
- Unicode arrows: ←, →, ↑, ↓

## Usage Example
To map F2 to switch to the previous tab (Ctrl+PageUp):
```ini
[keymapping]
mappings=F2=Ctrl+PageUp
```

To map multiple keys:
```ini
[keymapping]
mappings=F2=Ctrl+PageUp;F3=Ctrl+PageDown
```

## Technical Details
- Uses Windows low-level keyboard hook for interception
- Supports modifier keys (Ctrl, Shift, Alt)
- Properly releases keys in reverse order to avoid stuck keys
- Installed on Chrome++ initialization
- Uninstalled on Chrome++ shutdown

## Integration Points
1. **Initialization**: `InstallKeyMapping()` is called from `ChromePlus()` in `chrome++.cc`
2. **Cleanup**: `UninstallKeyMapping()` is called from `DllMain` on `DLL_PROCESS_DETACH`
3. **Configuration**: Reads from `[keymapping]` section in `chrome++.ini`

## Code Organization Benefits
The implementation follows the existing codebase structure:
- Separate header and implementation files
- Uses existing utility functions (StringSplit, GetIniString)
- Follows existing naming conventions
- Integrates cleanly with the main Chrome++ initialization flow
