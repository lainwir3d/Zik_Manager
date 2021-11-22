# Zik Manager

An unofficial native manager for the Parrot Zik headphones range (1 / 2.0 / 3).

# Disclaimer

This has been unmaintained for some years, and is likely to remain so because very few people use Zik nowadays. Source code is quite ugly, sorry about that.

# Features supported

- Noise control ON / OFF (Zik 1 / 2.0 / 3)
- Noise Cancelling / Street Mode advanced mode (Zik 2.0 / 3)
- Auto Noise Control (Zik 3)
- Spatialization / Concert Hall (Zik 1 / 2.0 / 3).
- Equalization / Thumb equalizer (Zik 2.0 / 3).
- Basic preset equalization (Zik 1)
- "Head detection" and "Auto connection" ON / OFF (Zik 1 / 2.0 / 3).
- Auto power off (Zik 1 / 2.0 / 3).
- "Flight mode" and "Text to Speech" ON / OFF (Zik 2.0 / 3).
- Bluetooth streaming delay setting (Zik 3)
- Demo mode to test the application without possessing the headphone.

Zik 1 owners, please do note again that the Zik 1 support is _VERY_ limited and will _NOT_ look like the store screenshots.

# Platforms supported

- Windows
- Linux
- OSX
- Sailfish (not sure if it's working anymore)
- Android

# Downloads

- [Windows](https://www.microsoft.com/fr-fr/p/zik-manager/9nmsfr8zgxv8)
- [OSX](https://apps.apple.com/fr/app/zik-manager/id1050660598)
- Android: no binaries yet, sorry.
- [Linux](https://github.com/lainwir3d/Zik_Manager/releases): Be careful, version is behind source code. I need to rebuild it using latest source.

# Limitations

On Android, only the first Zik headphones paired will work. Others Parrot products may cause problems with headphones detection.

# Building instructions

TODO, but a simple qmake && make should do it.

# FAQ

- Why does the app does not look the same as in the screenshot ?

 The app automatically switch layout when resized. Please resize the app in a "landscape" ratio to have the same results as the screenshot.

- Why does the app does not seems to have the same features as the screenshot ?

 If you have a Zik 2.0 or 3, the app will have all (or nearly all) features announced.

 HOWEVER if you have a Zik 1, as specified in the features above, only a few things are supported. Furthermore, there is no landscape mode for the Zik 1.
