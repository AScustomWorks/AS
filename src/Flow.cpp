//**************************************************************************************
//Flow module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"
//#include "dsp/digital.hpp"

struct Flow: Module {
    enum ParamIds {
        SWITCH_1,
        SWITCH_2,
        MODE_PARAM, 
        NUM_PARAMS
    };
    enum InputIds {
        INPUT_1,
        INPUT_2,
        RESET_1,
        RESET_2,
        CV_TRIG_INPUT_1,
        CV_TRIG_INPUT_2,
        NUM_INPUTS
    };
    enum OutputIds {
        OUTPUT_1,
        OUTPUT_2,
        NUM_OUTPUTS
    };
    enum LightIds {
        TRIGGER_LED_1,
        TRIGGER_LED_2,
        NUM_LIGHTS
    };

    dsp::SchmittTrigger btnTrigger1;
    dsp::SchmittTrigger extTrigger1;
    dsp::SchmittTrigger extReset1;
    dsp::SchmittTrigger btnTrigger2;
    dsp::SchmittTrigger extTrigger2;
    dsp::SchmittTrigger extReset2;

    bool on_1 = false;
    bool on_2 = false;
    bool light_inverted = false;

    float mute_fade1 =0.0f;
    float mute_fade2 =0.0f;
    const float fade_speed = 0.001f;

    Flow() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		//New in V2, config switches info without displaying values
		configSwitch(MODE_PARAM, 0.0f, 1.0f, 1.0f, "LED Mode", {"Inverted", "Regular"});
        configButton(SWITCH_1, "Switch 1");
        configButton(SWITCH_2, "Switch 2");
		//new V2, port labels
		//Inputs
		configInput(INPUT_1, "CH 1");
		configInput(INPUT_2, "CH 2");
        configInput(RESET_1, "CH 1 Reset");
        configInput(RESET_2, "CH 2 Reset");
        configInput(CV_TRIG_INPUT_1, "CH1 Trigger CV");
        configInput(CV_TRIG_INPUT_2, "CH2 Trigger CV");
		//Outputs
		configOutput(OUTPUT_1, "CH 1");
		configOutput(OUTPUT_2, "CH 2");

    }

    void process(const ProcessArgs &args) override {

        if (params[MODE_PARAM].getValue()){
            //switch lights turn on when the switch is enabled
            light_inverted = false;
        }else{
            //switch lights turn off when the switch is enabled
            light_inverted = true;
        }

        //TRIGGER 1
        if (btnTrigger1.process(params[SWITCH_1].getValue())||extTrigger1.process(inputs[CV_TRIG_INPUT_1].getVoltage())) {
            on_1 = !on_1; 
        }
        if (extReset1.process(inputs[RESET_1].getVoltage())) {
            on_1 = false; 
        }
    //SOFT MUTE/UNMUTE
        mute_fade1 -= on_1 ? fade_speed : -fade_speed;
        if ( mute_fade1 < 0.0f ) {
        mute_fade1 = 0.0f;
        } else if ( mute_fade1 > 1.0f ) {
        mute_fade1 = 1.0f;
        }
        outputs[OUTPUT_1].setVoltage(inputs[INPUT_1].getVoltage() * mute_fade1);
        if(light_inverted){
            lights[TRIGGER_LED_1].value = on_1 ? 0.0f : 1.0f;
        }else{
            lights[TRIGGER_LED_1].value = on_1 ? 1.0f : 0.0f;
        }
        
        //TRIGGER 2
        if (btnTrigger2.process(params[SWITCH_2].getValue())||extTrigger2.process(inputs[CV_TRIG_INPUT_2].getVoltage())) {
            on_2 = !on_2; 
        }
        if (extReset2.process(inputs[RESET_2].getVoltage())) {
            on_2 = false; 
        }
        //SOFT MUTE/UNMUTE
        mute_fade2 -= on_2 ? fade_speed : -fade_speed;
        if ( mute_fade2 < 0.0f ) {
        mute_fade2 = 0.0f;
        } else if ( mute_fade2 > 1.0f ) {
        mute_fade2 = 1.0f;
        }
        outputs[OUTPUT_2].setVoltage(inputs[INPUT_2].getVoltage() * mute_fade2);
        if(light_inverted){
            lights[TRIGGER_LED_2].value = on_2 ? 0.0f : 1.0f;
        }else{
            lights[TRIGGER_LED_2].value = on_2 ? 1.0f : 0.0f;
        }
        
    }

 
  	json_t *dataToJson()override {
		json_t *rootJm = json_object();

		json_t *on_statesJ = json_array();
		
			json_t *on_stateJ1 = json_integer((int) on_1);
			json_t *on_stateJ2 = json_integer((int) on_2);

			json_array_append_new(on_statesJ, on_stateJ1);
			json_array_append_new(on_statesJ, on_stateJ2);
		
		json_object_set_new(rootJm, "as_FlowStates", on_statesJ);

		return rootJm;
	}

	void dataFromJson(json_t *rootJm)override {
		json_t *on_statesJ = json_object_get(rootJm, "as_FlowStates");
		
			json_t *on_stateJ1 = json_array_get(on_statesJ, 0);
			json_t *on_stateJ2 = json_array_get(on_statesJ, 1);

			on_1 = !!json_integer_value(on_stateJ1);
			on_2 = !!json_integer_value(on_stateJ2);
		
	}
    
};


////////////////////////////////////
struct FlowWidget : ModuleWidget { 

    FlowWidget(Flow *module) {
        
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Flow.svg")));
    
        //SCREWS
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        //LED Mode: Regular/Inverted
        addParam(createParam<as_CKSS>(Vec(67, 23), module, Flow::MODE_PARAM));

        static const float led_offset = 6.0;//3.3;
        static const float led_center = 15;
        static const float y_offset = 150;
        //TRIGGER 1
        //SWITCH
/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 50), module, Flow::SWITCH_1));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 50+led_offset), module, Flow::TRIGGER_LED_1)); */
        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 50), module, Flow::SWITCH_1));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 50+led_offset), module, Flow::TRIGGER_LED_1));
        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(10, 140), module, Flow::CV_TRIG_INPUT_1));
        addInput(createInput<as_PJ301MPort>(Vec(55, 140), module, Flow::RESET_1));
        addInput(createInput<as_PJ301MPort>(Vec(10, 174), module, Flow::INPUT_1));
        addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 174), module, Flow::OUTPUT_1));
        //TRIGGER 2
        //SWITCH
/*         addParam(createParam<BigLEDBezel>(Vec(led_center, 50+y_offset), module, Flow::SWITCH_2));
        addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 50+led_offset+y_offset), module, Flow::TRIGGER_LED_2)); */
        addParam(createParam<JumboLEDBezel60>(Vec(led_center, 50+y_offset), module, Flow::SWITCH_2));
        addChild(createLight<JumboLedLight60<RedLight>>(Vec(led_center+led_offset, 50+led_offset+y_offset), module, Flow::TRIGGER_LED_2));
        //PORTS
        addInput(createInput<as_PJ301MPort>(Vec(10, 140+y_offset), module, Flow::CV_TRIG_INPUT_2));
        addInput(createInput<as_PJ301MPort>(Vec(55, 140+y_offset), module, Flow::RESET_2));
        addInput(createInput<as_PJ301MPort>(Vec(10, 174+y_offset), module, Flow::INPUT_2));
        addOutput(createOutput<as_PJ301MPortGold>(Vec(55, 174+y_offset), module, Flow::OUTPUT_2));

    }
};

Model *modelFlow = createModel<Flow, FlowWidget>("Flow");