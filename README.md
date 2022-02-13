# Unreal Audio DSP Collection (UE5)

## WARNING: BEWARE THAT THE METASOUND CODE IS BASED ON UE5 EARLY ACCESS AND THE API/FUNCTIONALITY WILL CHANGE ONCE UE5 IS FINALLY RELEASED. THIS PROJECT WILL BE UPDATED ACCORDINGLY WHEN THAT HAPPENS.

![](https://user-images.githubusercontent.com/7047334/153762477-fa270440-b544-4b58-b36e-e4dbcfd52eff.png)

An Unreal 5 plugin with a collection of audio effects in 3 different flavors: **Metasound Node**, **SourceEffect** and **SubmixEffect**.

Currently implemented Effects:
- Gain
- Saturation (A.K.A. Drive, Distortion, Wave Shaper) (WIP, more saturation types will be added)

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
![](https://user-images.githubusercontent.com/7047334/153762718-c8993a63-ee35-4e2f-b8f0-ef5ea64da936.gif)

**SourceEffects** and **SubmixEffects:**
<br/>
![](https://user-images.githubusercontent.com/7047334/153764367-56818e60-eb33-4430-a6c6-ea74028368f7.gif)
