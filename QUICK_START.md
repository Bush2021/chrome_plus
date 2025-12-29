# Chrome++ Key Mapping Feature - Quick Start

## What Was Done

### ✅ Implemented Key Mapping Feature
A new feature that allows users to remap keyboard keys to other key combinations using a low-level keyboard hook.

### ✅ Extracted Reusable Logic  
Created `ParseKeyString()` utility function to eliminate duplicate code between hotkey and keymapping modules.

### ✅ Improved Code Organization
Refactored both `hotkey.cc` and `keymapping.cc` to use shared parsing logic, making the codebase more maintainable.

## Quick Usage

### Step 1: Edit chrome++.ini
Add a `[keymapping]` section:

```ini
[keymapping]
mappings=F2=Ctrl+PageUp;F3=Ctrl+PageDown
```

### Step 2: Restart Chrome
The key mappings will be active immediately.

### Example: Map F2 to Previous Tab
```ini
[keymapping]
mappings=F2=Ctrl+PageUp
```

Now pressing F2 will switch to the previous tab (same as Ctrl+PageUp).

## Configuration Examples

### Switch Tabs with F2/F3
```ini
[keymapping]
mappings=F2=Ctrl+PageUp;F3=Ctrl+PageDown
```

### Close Tab with F4
```ini
[keymapping]
mappings=F4=Ctrl+W
```

### Reopen Closed Tab with F5
```ini
[keymapping]
mappings=F5=Ctrl+Shift+T
```

### Multiple Mappings
```ini
[keymapping]
mappings=F2=Ctrl+PageUp;F3=Ctrl+PageDown;F4=Ctrl+W;F5=Ctrl+Shift+T
```

## Files Changed

**New Files:**
- `src/keymapping.h` - Interface
- `src/keymapping.cc` - Implementation

**Modified Files:**
- `src/chrome++.cc` - Integration
- `src/utils.h` - Added ParseKeyString
- `src/utils.cc` - Implemented ParseKeyString  
- `src/hotkey.cc` - Refactored to use ParseKeyString

**Documentation:**
- `IMPLEMENTATION_SUMMARY.md` - Complete technical documentation
- `KEYMAPPING_IMPLEMENTATION.md` - Feature-specific documentation
- `keymapping_example.ini` - Configuration examples

## Build Instructions

Build the project using xmake:
```bash
xmake build
```

The output DLL (`version.dll`) will be in the build directory.

## Notes

- Key mapping uses a low-level keyboard hook
- Mappings are loaded once at Chrome++ startup
- To change mappings, edit the INI file and restart Chrome
- The feature has no performance impact when not configured
- Original key functionality is blocked when mapped

## Supported Keys

See `keymapping_example.ini` or `KEYMAPPING_IMPLEMENTATION.md` for the complete list of supported keys.
