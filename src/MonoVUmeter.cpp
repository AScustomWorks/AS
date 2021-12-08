//***********************************************************************************************
//
//MonoVUmeter module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//  
//***********************************************************************************************

#include "AS.hpp"
//#include "dsp/vumeter.hpp"

struct MonoVUmeter : Module {
	enum ParamIds {
	 	NUM_PARAMS
	};

	enum InputIds {
		INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUT,
	 	NUM_OUTPUTS
	};
	
	enum LightIds {
		METER_LIGHT,
		NUM_LIGHTS = METER_LIGHT+15
	};

	MonoVUmeter() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configInput(INPUT, "Audio");
		//Outputs
		configOutput(OUT, "Audio");	

	}

	dsp::VuMeter vuBar;

	void process(const ProcessArgs &args) override {
		//GET VALUES AND ROUTE SIGNAL TO OUTPUT
		float signal_in = inputs[INPUT].getVoltage();
		outputs[OUT].setVoltage(signal_in);
		//VU METER BARS LIGHTS
		vuBar.dBInterval = 3;
		vuBar.setValue(signal_in / 10.0f);
		for (int i = 0; i < 15; i++){
			lights[METER_LIGHT + i].setSmoothBrightness(vuBar.getBrightness(i), args.sampleTime);
	}
};

};


struct MonoVUmeterWidget : ModuleWidget { 

	MonoVUmeterWidget(MonoVUmeter *module) {
		
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MonoVUmeter.svg")));
	
		//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
		addChild(createWidget<as_HexScrew>(Vec(0, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// LEFT COLUMN LEDs
		static const float ledCol = 11;
		static const float offsetY = 12;
		static const float startY = 66;
			addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 0), module, MonoVUmeter::METER_LIGHT + 0));
			addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 1), module, MonoVUmeter::METER_LIGHT + 1));
			addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 2), module, MonoVUmeter::METER_LIGHT + 2));
		addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 3), module, MonoVUmeter::METER_LIGHT + 3));
		addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 4), module, MonoVUmeter::METER_LIGHT + 4));
		addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 5), module, MonoVUmeter::METER_LIGHT + 5));
		addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 6), module, MonoVUmeter::METER_LIGHT + 6));
		addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 7), module, MonoVUmeter::METER_LIGHT + 7));
		addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 8), module, MonoVUmeter::METER_LIGHT + 8));
		addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 9), module, MonoVUmeter::METER_LIGHT + 9));
		addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 10), module, MonoVUmeter::METER_LIGHT + 10));
		addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 11), module, MonoVUmeter::METER_LIGHT + 11));
		addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 12), module, MonoVUmeter::METER_LIGHT + 12));
		addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 13), module, MonoVUmeter::METER_LIGHT + 13));
		addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 14), module, MonoVUmeter::METER_LIGHT + 14));
		
		//INPUTS
		addInput(createInput<as_PJ301MPort>(Vec(3, 270), module, MonoVUmeter::INPUT));
		//OUTPUTS
		addOutput(createOutput<as_PJ301MPortGold>(Vec(3,310), module, MonoVUmeter::OUT));
	}
};


Model *modelMonoVUmeter = createModel<MonoVUmeter, MonoVUmeterWidget>("MonoVUmeter");