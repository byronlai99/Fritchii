# Fritchii-HDrum Slim

This development branch focuses on running already-decrypted game content.

## What's changed

- Removed external console key management and the `prod.keys` / `title.keys` import workflow.
- Removed Ticket, title-key and console-key derivation paths.
- Disabled encrypted NCA and NAX loading.
- Kept NUP, ExeFS and RomFS loading for decrypted game content.
- Kept the emulator's internal AES and system-service cryptography needed by game code.

Encrypted retail game packages are not supported on this branch. Use a decrypted NUP file or an extracted game directory.

This is an early slim-branch build. Compatibility and performance work remains focused on Ocarina of Time first.
