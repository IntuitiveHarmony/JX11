#  🎹 JX11

🚧 THIS PROJECT IS A WORK IN PROGRESS 🚧

<hr/>
<br/>

## 📚 New Book, New Project

I recently came across this book, [*Code Your Own Synth Plug-Ins With C++ and JUCE*](https://leanpub.com/synth-plugin "Checkout the Book"), by [Matthijs Hollemans](https://github.com/hollance "Checkout the Author's GitHub") .  It goes through how to build a project synth he calls JX11, which itself is modeled after an open source plugin called JX10 built by MDA (if you find a link that works let me know 😎).  What really attracted me to this book was how the author was trying to take a complicated subject like DSP and building a syntheziser and make it accesable to a beginner like me.  While I am well on my way to being a solid programmer I still have less than a year's worth of experience codding at this point.  C++ is still pretty new to me as well, being the most recent addition to my coding languages.  I started to learn programming with javaScript and even toyed with the [tone.js](https://tonejs.github.io/ "Tone.js Docs") library a bit, until it dawned on me, people use DAWs to make music, not web browsers.  This brought me to plug-ins, then to C++ and the JUCE framework.

In order to gain some fundamental C++ skills i enrolled in Coursera's C++ specilization, which I just finished recently.  Building the JX11 seemed like the next logical step in my journey into sound.  Before I purchased the book I went to the finished source [code](https://github.com/hollance/synth-plugin-book#code-your-own-synth-plug-ins-with-c-and-juce "Synth-Plugin-Book Repo") and cloned it to my local machine.  I wanted to see if I could get this project running on my computer.  When I went to compile I immedieletly ran into errors.  Something about an ambigous reference I think...  At first I thought it may have been that I was using JUCE 6.1.6 so I updated to version 7.0.5 as the book says it goes off version 7.  The issue still presisted so after a bit of googling I finally cleared up an issue with the namespace, and the project was able to compile on my computer.  Once I heard the JX11 playing through my speakers I just had to build it.  Not because of it's sound, no, (although it does sound pretty good).  It was the fact that I was able to perform some C++ debugging on my own.     
