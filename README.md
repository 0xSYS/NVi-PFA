# NVirsual Piano From Above
A clone of the original [Pano From Above](https://github.com/brian-pantano/PianoFromAbove) made by Brian Panatano.
Now available on on android.

This clone is powered by Qishipai's [midi processing library](https://github.com/qishipai/NVirsual) and using SDL3 to render the notes and the keyboard.

> [!WARNING]
> This project is still in early development and any release could be unstable. Consider building and testing them at your own risk!!

## Features
- [X] Default midi and soundfont
- [X] Custom background color support
- [X] Custom themed GUI
- [X] Settings storage to file (.toml)
- [X] Voice count settings for bass stream
- [X] Midi and soundfont list providers
- [X] Play / Pause playback
- [X] Seek forward
- [ ] Seek backwards
- [X] Live Note speed change
- [ ] Improving the audio limiter
- [ ] Better UI layout on mobile
- [ ] File information support
- [ ] Custom note color array support
- [ ] Veritcal lines on the background
- [ ] Background image support
- [ ] Common midi and soundfont reloading (This must not require app restarting after each midi playing session)
- [ ] Seting for custom folders / directories to search for midi and soundfont files
- [ ] More audio settings (like Realtime frame rate, frame fluctuation, enabling disabling limiter)
- [ ] Performance improvements
- [ ] Loging to text file (This can be useful for development purposes)
- [X] Custom icon and app name

# How to build an apk
First you need to make sure you have the android studio and android-sdk installed.

2) Git clone this repo and cd into it.
3)
```
./gradlew clean
./gradlew assembleDebug
```
This only builds the debug version of the apk

*) If you want to build the desktop version (which is currently available and works best on arch linux atm) check out [the guide build](app/jni/src/README.md) from the core source code of the app.






### All Contributors
- Qishipai: provider of his c++ midi processing library
- Tweak: Creating the PFA imitation and upgrading to SDL3, implementing a simple overlap remover
- 0xsys: Most UI design, settings and translations
- Nerdly: UI Font Choice
- Hex: Playback options: play / pause, seek backwards/forwards
- Zeal: Custom Icon
