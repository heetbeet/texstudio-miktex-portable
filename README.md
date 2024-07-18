A pattern for a portable TexStudio and MikTex.

### Download one of the releases
https://github.com/heetbeet/texstudio-miktex-portable/releases

### Or make your own
Fill the program folders with these:
- https://miktex.org/howto/portable-edition
- https://www.texstudio.org (search for portable version)

And correct the paths within the `bin\*.cmd` files to point to the correct locations.

### Exe entry points
The .exe entry points are created by running `compile-cmd-wrappers.cmd` to output `MikTex.exe` and `TexStudio.exe` and then using the program ResourceHacker to add the appropriate icons from the `assets` directory.