//***********************************************************************************************
//
//MonoVUmeter module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//  
//***********************************************************************************************

#include "AS.hpp"
#include "dsp/vumeter.hpp"

struct monoVUmeter : Module {
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

	monoVUmeter() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
	}
	void step() override;

	VUMeter vuBar;

};

void monoVUmeter::step(){
	//GET VALUES AND ROUTE SIGNAL TO OUTPUT
	float signal_in = inputs[INPUT].value;
	outputs[OUT].value = signal_in;
	//VU METER BARS LIGHTS
	vuBar.dBInterval = 3;
	vuBar.setValue(signal_in / 10.0f);
	for (int i = 0; i < 15; i++){
		lights[METER_LIGHT + i].setBrightnessSmooth(vuBar.getBrightness(i));
	}
};

monoVUmeterWidget::monoVUmeterWidget(){
	monoVUmeter *module = new monoVUmeter();
	setModule(module);
	box.size = Vec(2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/monoVUmeter.svg")));
		addChild(panel);
	}
	//SCREWS - SPECIAL SPACING FOR RACK WIDTH*4
	addChild(createScrew<as_HexScrew>(Vec(0, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	// LEFT COLUMN LEDs
	static const float ledCol = 11;
	static const float offsetY = 12;
	static const float startY = 66;
		addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 0), module, monoVUmeter::METER_LIGHT + 0));
		addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 1), module, monoVUmeter::METER_LIGHT + 1));
		addChild(createLight<MeterLight<RedLight>>(Vec(ledCol, startY + offsetY * 2), module, monoVUmeter::METER_LIGHT + 2));
	addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 3), module, monoVUmeter::METER_LIGHT + 3));
	addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 4), module, monoVUmeter::METER_LIGHT + 4));
	addChild(createLight<MeterLight<OrangeLight>>(Vec(ledCol, startY + offsetY * 5), module, monoVUmeter::METER_LIGHT + 5));
	addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 6), module, monoVUmeter::METER_LIGHT + 6));
	addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 7), module, monoVUmeter::METER_LIGHT + 7));
	addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 8), module, monoVUmeter::METER_LIGHT + 8));
	addChild(createLight<MeterLight<YellowLight>>(Vec(ledCol, startY + offsetY * 9), module, monoVUmeter::METER_LIGHT + 9));
	addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 10), module, monoVUmeter::METER_LIGHT + 10));
	addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 11), module, monoVUmeter::METER_LIGHT + 11));
	addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 12), module, monoVUmeter::METER_LIGHT + 12));
	addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 13), module, monoVUmeter::METER_LIGHT + 13));
	addChild(createLight<MeterLight<GreenLight>>(Vec(ledCol, startY + offsetY * 14), module, monoVUmeter::METER_LIGHT + 14));
	
	//INPUTS
	addInput(createInput<as_PJ301MPort>(Vec(3, 270), module, monoVUmeter::INPUT));
	//OUTPUTS
	addOutput(createOutput<as_PJ301MPort>(Vec(3,307), module, monoVUmeter::OUT));
}