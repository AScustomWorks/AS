//***********************************************************************************************
//
//SuperDriveFx module for VCV Rack by Alfredo Santamaria  - AS - https://github.com/AScustomWorks/AS
//Variable-hardness clipping code from scoofy[ AT ]inf[ DOT ]elte[ DOT ]hu
//Filter code from from VCV rack dsp
//
//***********************************************************************************************

#include "AS.hpp"
#include "dsp/digital.hpp"
#include "dsp/filter.hpp"

//#include <stdlib.h>

struct SuperDriveFx : Module{
	enum ParamIds {
		DRIVE_PARAM,
		OUTPUT_GAIN_PARAM,
		TONE_PARAM,
        BYPASS_SWITCH,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNAL_INPUT,
		DRIVE_CV_INPUT,
		GAIN_CV_INPUT,
		TONE_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SIGNAL_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		GAIN_LIGHT,
		TONE_LIGHT,
		DRIVE_LIGHT,
		BYPASS_LED,
		NUM_LIGHTS
	};

	SchmittTrigger bypass_button_trig;
	int drive_scale=50;//to handle cv parameters properly

	RCFilter lowpassFilter;
	RCFilter highpassFilter;

	bool fx_bypass = false;
	SuperDriveFx() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override;

	json_t *toJson()override {
		json_t *rootJm = json_object();

		json_t *statesJ = json_array();
		
			json_t *bypassJ = json_boolean(fx_bypass);
			json_array_append_new(statesJ, bypassJ);
		
		json_object_set_new(rootJm, "as_FxBypass", statesJ);

		return rootJm;
	}

	void fromJson(json_t *rootJm)override {
		json_t *statesJ = json_object_get(rootJm, "as_FxBypass");
		
			json_t *bypassJ = json_array_get(statesJ, 0);

			fx_bypass = !!json_boolean_value(bypassJ);
		
	}
	
	float input_signal=0.0;
	float drive = 0.1;
	float process= 0.0;
	float inv_atan_drive = 0.0;
	float output_signal= 0.0;
	
};

void SuperDriveFx::step() {

  if (bypass_button_trig.process(params[BYPASS_SWITCH].value))
    {
		  fx_bypass = !fx_bypass;
	  }
    lights[BYPASS_LED].value = fx_bypass ? 1.0 : 0.0;

	float input_signal = inputs[SIGNAL_INPUT].value;
	//OVERDRIVE SIGNAL
	//float drive = params[DRIVE_PARAM].value;
	drive = clampf(params[DRIVE_PARAM].value + inputs[DRIVE_CV_INPUT].value / 10.0, 0.1, 1.0);

	drive = drive * drive_scale;
	//precalc
	inv_atan_drive = 1.0/atan(drive);
	//process
	process = inv_atan_drive * atan(input_signal*drive);
	//output_signal = process * params[OUTPUT_GAIN_PARAM].value;
	output_signal = process * clampf(params[OUTPUT_GAIN_PARAM].value + inputs[GAIN_CV_INPUT].value / 10.0, 0.0, 1.0);

	//TONE CONTROL
	float tone = clampf(params[TONE_PARAM].value + inputs[TONE_CV_INPUT].value / 10.0, 0.0, 1.0);
	float lowpassFreq = 10000.0 * powf(10.0, clampf(2.0*tone, 0.0, 1.0));
	lowpassFilter.setCutoff(lowpassFreq / engineGetSampleRate());
	lowpassFilter.process(output_signal);
	output_signal = lowpassFilter.lowpass();
	float highpassFreq = 10.0 * powf(100.0, clampf(2.0*tone - 1.0, 0.0, 1.0));
	highpassFilter.setCutoff(highpassFreq / engineGetSampleRate());
	highpassFilter.process(output_signal);
	output_signal = highpassFilter.highpass();

	//check bypass switch status
	if (fx_bypass){
		outputs[SIGNAL_OUTPUT].value = inputs[SIGNAL_INPUT].value;
	}else {
		outputs[SIGNAL_OUTPUT].value = output_signal*3.5;// 3.5;
	}
	//lights without cv input - old
	//lights[DRIVE_LIGHT].value = params[DRIVE_PARAM].value;
	//lights[GAIN_LIGHT].value = params[OUTPUT_GAIN_PARAM].value;

	lights[DRIVE_LIGHT].value = clampf(params[DRIVE_PARAM].value + inputs[DRIVE_CV_INPUT].value / 10.0, 0.0, 1.0);
	lights[TONE_LIGHT].value = clampf(params[TONE_PARAM].value + inputs[TONE_CV_INPUT].value / 10.0, 0.0, 1.0);
	lights[GAIN_LIGHT].value = clampf(params[OUTPUT_GAIN_PARAM].value + inputs[GAIN_CV_INPUT].value / 10.0, 0.0, 1.0);

}

SuperDriveFxWidget::SuperDriveFxWidget() {
	SuperDriveFx *module = new SuperDriveFx();
	setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;	
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/SuperDrive.svg")));
		addChild(panel); 
	}

 	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //KNOBS  
	addParam(createParam<as_FxKnobWhite>(Vec(43, 60), module, SuperDriveFx::DRIVE_PARAM, 0.1, 1, 0.1));
	addParam(createParam<as_FxKnobWhite>(Vec(43, 125), module, SuperDriveFx::TONE_PARAM, 0, 1, 0.5));
	addParam(createParam<as_FxKnobWhite>(Vec(43, 190), module, SuperDriveFx::OUTPUT_GAIN_PARAM, 0.0, 1.0, 0.5));
	//LIGHTS
	addChild(createLight<SmallLight<YellowLight>>(Vec(39, 57), module, SuperDriveFx::DRIVE_LIGHT));
	addChild(createLight<SmallLight<YellowLight>>(Vec(39, 122), module, SuperDriveFx::TONE_LIGHT));
	addChild(createLight<SmallLight<YellowLight>>(Vec(39, 187), module, SuperDriveFx::GAIN_LIGHT));
    //BYPASS SWITCH
  	addParam(createParam<LEDBezel>(Vec(33, 260), module, SuperDriveFx::BYPASS_SWITCH , 0.0, 1.0, 0.0));
  	addChild(createLight<LedLight<RedLight>>(Vec(35.2, 262), module, SuperDriveFx::BYPASS_LED));
    //INS/OUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 310), module, SuperDriveFx::SIGNAL_INPUT));
	addOutput(createOutput<as_PJ301MPort>(Vec(55, 310), module, SuperDriveFx::SIGNAL_OUTPUT));
	//CV INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(10, 67), module, SuperDriveFx::DRIVE_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(10, 132), module, SuperDriveFx::TONE_CV_INPUT));
	addInput(createInput<as_PJ301MPort>(Vec(10, 197), module, SuperDriveFx::GAIN_CV_INPUT));
 
}
