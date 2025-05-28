The Curl and Skew Generator (CSG) is a monophonic harsh noise synthesizer developed in JUCE. It consists of a single chaotic oscillator which affects itself by feedback-based frequency- and phase- modulation. This oscillator then gets routed to an extremely roudy state-variable filter.
<br>
CSG has 5 sections: FM, PM, VCF, LFO, and 2-stage envelope.
Some reasons it sounds so insane: <br>
-FM feedback is passed through a variable delay buffer (MEMORY) <br>
-FM feedback is bit-crushed (FM DEGRADE) and smoothed (FM SMOOTH) <br>
-PM feedback is sin/cosine shaped (PM SHAPE), bitcrushed (PM DEGRADE), and tamed (PM TAME) <br>
-The state-variable filter is a variant of a traditional SVF design, adding continuous nonlinearity in the resonance path. It is implemented using the Runge-Kutta method and sounds plausibly analog. <br>

This project was built using JUCE 7.

Current limitations: <br>
-Single modulation source (LFO) can be variably routed to most parameters. The parameters currently not working for modulation include Filter Drive, Filter Type, Memory, LFO Rate, LFO Shape, Drone, Rise, and Fall. These mod parameters will not allow you to adjust their knob, and a tooltip will inform you that the parameter has not been implemented yet.
 <br>
-Single 2-stage (RISE/FALL) envelope, only affects amplitude <br>

CSG was [presented](https://www.youtube.com/watch?v=okdVPLvdnEc) at the Heretical Sound Synthesis in Helsinki. <br>

![csg](./csg.png "csg standalone")