#include "AS.hpp"


// The plugin-wide instance of the Plugin class
Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	// This is the unique identifier for your plugin
	p->slug = "AS";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->website = "http://www.hakken.com.mx";
	p->manual = "";

	// For each module, specify the ModuleWidget subclass, manufacturer slug (for saving in patches), manufacturer human-readable name, module slug, and module name
	//OSCILLATORS
	p->addModel(createModel<SawOscWidget>("AS", "SawOSC", "Saw OSC", OSCILLATOR_TAG));
	p->addModel(createModel<SineOscWidget>("AS", "SineOSC", "Sine OSC", OSCILLATOR_TAG));
	//TOOLS
	p->addModel(createModel<ADSRWidget>("AS", "ADSR", "ADSR", ENVELOPE_GENERATOR_TAG));
	p->addModel(createModel<BPMClockWidget>("AS", "BPMClock", "BPM Clock", CLOCK_TAG));
	p->addModel(createModel<SEQ16Widget>("AS", "SEQ16", "16 Step Sequencer", SEQUENCER_TAG));
	p->addModel(createModel<Mixer8chWidget>("AS", "Mixer8ch", "8 CH Mixer", MIXER_TAG));
	p->addModel(createModel<Multiple2_5Widget>("AS", "Multiple2_5", "Multiple 2 x 5", MULTIPLE_TAG));
	p->addModel(createModel<TriggersWidget>("AS", "Triggers", "Triggers", SWITCH_TAG));
	//EFFECTS
	p->addModel(createModel<DelayPlusFxWidget>("AS", "DelayPlusFx", "FX Delay Plus", DELAY_TAG));
	p->addModel(createModel<PhaserFxWidget>("AS", "PhaserFx", "FX Phaser", EFFECT_TAG));
	p->addModel(createModel<WaveShaperWidget>("AS", "WaveShaper", "FX Wave Shaper", EFFECT_TAG));
	//BLANK PANELS
	p->addModel(createModel<BlankPanel4Widget>("AS", "BlankPanel4", "Blank Panel 4", BLANK_TAG));
	p->addModel(createModel<BlankPanel6Widget>("AS", "BlankPanel6", "Blank Panel 6", BLANK_TAG));
	p->addModel(createModel<BlankPanel8Widget>("AS", "BlankPanel8", "Blank Panel 8", BLANK_TAG));

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
