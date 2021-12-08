#include "AS.hpp"


struct AtNuVrTr : Module {
	enum ParamIds {
		ATEN1_PARAM,
		OFFSET1_PARAM,
		ATEN2_PARAM,
		OFFSET2_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CV_ATEN_1,
		CV_ATEN_2,
		CV_OFFSET_1,
		CV_OFFSET_2,
		IN1_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		OUT1_POS_LIGHT,
		OUT1_NEG_LIGHT,
		OUT2_POS_LIGHT,
		OUT2_NEG_LIGHT,
		NUM_LIGHTS
	};

	AtNuVrTr() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(AtNuVrTr::ATEN1_PARAM, -1.0f, 1.0f, 0.0f, "CH 1 gain");
		configParam(AtNuVrTr::OFFSET1_PARAM, -10.0f, 10.0f, 0.0f, "CH 1 offset", " V");
		configParam(AtNuVrTr::ATEN2_PARAM, -1.0f, 1.0f, 0.0f, "CH 2 gain");
		configParam(AtNuVrTr::OFFSET2_PARAM, -10.0f, 10.0f, 0.0f, "CH 2 offset", " V");

		//new V2, port labels
		//inputs
		configInput(CV_ATEN_1, "Gain Mod CV 1");
		configInput(CV_OFFSET_1, "Offset CV 1");
		configInput(IN1_INPUT, "CV 1");

		configInput(CV_ATEN_2, "Gain Mod CV 2");
		configInput(CV_OFFSET_2, "Offset CV 2");
		configInput(IN2_INPUT, "CV 2");
		//Outputs
		configOutput(OUT1_OUTPUT, "CV 1");
		configOutput(OUT2_OUTPUT, "CV 2");
		
	}
	
	void process(const ProcessArgs &args) override {
		float cv_at1 = 0.0f;
		if(inputs[CV_ATEN_1].isConnected()){
			cv_at1 = rescale(inputs[CV_ATEN_1].getVoltage(), -10.0f,10.0f, -1.0f, 1.0f);
		}
		float cv_off1 = 0.0f;
		if(inputs[CV_OFFSET_1].isConnected()){
			cv_off1 = rescale(inputs[CV_OFFSET_1].getVoltage(), -10.0f,10.0f, -10.0f, 10.0f);
		}
		float atten1 = params[ATEN1_PARAM].getValue() + cv_at1;
		float offset1 = params[OFFSET1_PARAM].getValue() + cv_off1;
		float out1 = inputs[IN1_INPUT].getVoltage() * atten1 + offset1;

		float cv_at2 = 0.0f;
		if(inputs[CV_ATEN_2].isConnected()){
			cv_at2 = rescale(inputs[CV_ATEN_2].getVoltage(), -10.0f,10.0f, -1.0f, 1.0f);
		}
		float cv_off2 = 0.0f;
		if(inputs[CV_OFFSET_2].isConnected()){
			cv_off2 = rescale(inputs[CV_OFFSET_2].getVoltage(), -10.0f,10.0f, -10.0f, 10.0f);
		}
		float atten2 = params[ATEN2_PARAM].getValue() + cv_at2;
		float offset2 = params[OFFSET2_PARAM].getValue() + cv_off2;
		float out2 = inputs[IN2_INPUT].getVoltage() * atten2 + offset2;


		out1 = clamp(out1, -10.0f, 10.0f);
		out2 = clamp(out2, -10.0f, 10.0f);

		outputs[OUT1_OUTPUT].setVoltage(out1);
		outputs[OUT2_OUTPUT].setVoltage(out2);
		lights[OUT1_POS_LIGHT].value = fmaxf(0.0f, out1 / 5.0f);
		lights[OUT1_NEG_LIGHT].value = fmaxf(0.0f, -out1 / 5.0f);
		lights[OUT2_POS_LIGHT].value = fmaxf(0.0f, out2 / 5.0f);
		lights[OUT2_NEG_LIGHT].value = fmaxf(0.0f, -out2 / 5.0f);
	}
};


struct AtNuVrTrWidget : ModuleWidget {
	AtNuVrTrWidget(AtNuVrTr *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AtNuVrTr.svg")));

		//SCREWS
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const int group_offset = 160;
		//ATTN 1
		addParam(createParam<as_KnobBlack>(Vec(34, 45), module, AtNuVrTr::ATEN1_PARAM));
		addParam(createParam<as_Knob>(Vec(34, 100), module, AtNuVrTr::OFFSET1_PARAM));

		addChild(createLight<SmallLight<GreenRedLight>>(Vec(65, 95), module, AtNuVrTr::OUT1_POS_LIGHT));

		addInput(createInput<as_PJ301MPort>(Vec(4, 51), module, AtNuVrTr::CV_ATEN_1));
		addInput(createInput<as_PJ301MPort>(Vec(4, 106), module, AtNuVrTr::CV_OFFSET_1));

		addInput(createInput<as_PJ301MPort>(Vec(8, 165), module, AtNuVrTr::IN1_INPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(43, 165), module, AtNuVrTr::OUT1_OUTPUT));
		//ATTN 2
		addParam(createParam<as_KnobBlack>(Vec(34, 45+group_offset), module, AtNuVrTr::ATEN2_PARAM));
		addParam(createParam<as_Knob>(Vec(34, 100+group_offset), module, AtNuVrTr::OFFSET2_PARAM));

		addChild(createLight<SmallLight<GreenRedLight>>(Vec(65, 95+group_offset), module, AtNuVrTr::OUT2_POS_LIGHT));

		addInput(createInput<as_PJ301MPort>(Vec(4, 51+group_offset), module, AtNuVrTr::CV_ATEN_2));
		addInput(createInput<as_PJ301MPort>(Vec(4, 106+group_offset), module, AtNuVrTr::CV_OFFSET_2));

		addInput(createInput<as_PJ301MPort>(Vec(8, 165+group_offset), module, AtNuVrTr::IN2_INPUT));
		addOutput(createOutput<as_PJ301MPortGold>(Vec(43, 165+group_offset), module, AtNuVrTr::OUT2_OUTPUT));

	}
};


Model *modelAtNuVrTr = createModel<AtNuVrTr, AtNuVrTrWidget>("AtNuVrTr");
