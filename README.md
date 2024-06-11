# Mako Chorus VST
* JUCE VST3 Guitar Speaker Sim.
* Demo JUCE program.
* Tested on Windows only.
* Written in Visual C++ 2022.
* Written for new programmers, not complicated.
* Version: 1.00
* Posted: June 11, 2024

VERSION
------------------------------------------------------------------
1.00 - Initial release.

DISCLAIMER
------------------------------------------------------------------  
This VST was written in a very simple way. Just simple code for
people who are not programmers but want to dabble in the magic of JUCE VSTs.

If you are new to JUCE code, the Mako Thump VST may be a better
starting place. Or be brave and try this.
       
SUMMARY
------------------------------------------------------------------
A Juce/C++ VST3 written to create a chorus effect. 

![Demo Image](docs/assets/makochorusdemo01.png)

# THEORY OF OPERATION<br />
CHORUS EFFECT
A Chorus effect is designed to make a voice/musical instrument sound like more than one
device is being played simultaneously. It does this by creating a slightly delayed echo
of the original sound. The amount of time between the original sound and the echo is
constantly changed. 

The start and stop delay times are usually in the 15 to 20 mS range. These means that the
core effect is working in the 50 to 67 Hz frequency range of the original signal.

The difference in the start and stop times is called DEPTH in most chorus effect units.

How fast the echo is changed from the start and stop time is called RATE in most units.

FLANGER EFFECT  
A Flanger is very similar to a chorus effect, but uses mush shorter start and stop delay times.
