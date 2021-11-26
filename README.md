# systemwide-vst

Systemwide VST host that allows audio processing like the Sonarworks Systemwide Application.

## Install Dependencies

1) Get [JUCE](https://juce.com/get-juce/download)
2) Clone `https://github.com/dmikushin/tray.git`
3) Install apt packages:
```shell script
sudo apt install \
  libgtk-3-dev \
  libappindicator3-dev gir1.2-appindicator3-0.1 \
  libwebkit2gtk-4.0-dev \
  libasound2-dev \
  libjack-dev \ 
  ladspa-sdk
```

## Configure

1) Open the project in the Projucer
2) Make sure to correctly link `tray` (cog icon at the top left, then "Header Search Paths")