# Unreal Audio DSP Collection (UE5)

#### `[Current main branch version: UE5.2, for other versions check repo branches]`

![](https://user-images.githubusercontent.com/7047334/157249193-f3bbc3ca-d5ad-4e6f-a86b-b728568a224e.png)

An Unreal 5 plugin with a collection of audio effects in 3 different flavors: **Metasound Node**, **SourceEffect** and **SubmixEffect**.

Currently implemented Effects:
- Gain
- Saturation (A.K.A. Drive, Distortion, Wave Shaper)

### Build steps:
- **Clone** repository
- Right click ***UEAudioDSPCollection.uproject*** > *Generate Visual Studio projects files*
- Open ***UEAudioDSPCollection.sln***
- **Build** Solution


### Testing the plugin:
- Open ***UEAudioDSPCollection.uproject*** (or start it from Visual Studio)
- **DemoMap** will be automatically loaded
- **Metasound:**
    - Open ***Effect*Node_Metasound asset**
    - Click ***Play*** button to start the Metasound
- **SourceEffect** and **SubmixEffect:**
    - Go to ***Blueprints*** > ***Open Level Blueprint***
    - You will see that there is Blueprint logic to trigger a sound at the Sphere location and a Timeline that will drive a parameter of the SourceEffect
    - If you want to test the Submix effect simply connect the Update port from Timeline to the Set Settings node of the SubmixEffect
    - Click ***Play*** button to start the Level

<br/>

**Metasound Nodes:**
<br/>
![](https://user-images.githubusercontent.com/7047334/157324269-b15c6576-a2e8-44e4-ae47-749d0e74e795.gif)

**SourceEffects** and **SubmixEffects:**
<br/>
![](https://user-images.githubusercontent.com/7047334/153764367-56818e60-eb33-4430-a6c6-ea74028368f7.gif)
