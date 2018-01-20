//**************************************************************************************
//Triggers module for VCV Rack by Alfredo Santamaria - AS - https://github.com/AScustomWorks/AS
//
//**************************************************************************************
#include "AS.hpp"
#include "dsp/digital.hpp"
#include <iostream>
#include <cmath>

struct Triggers: Module {
    enum ParamIds {
        VOLTAGE_PARAM,
        RUN_SWITCH,
        MOMENTARY_SWITCH,
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        RUN_OUTPUT1,
        RUN_OUTPUT2,
        RUN_OUTPUT3,
        RUN_OUTPUT4,
        MOMENTARY_OUT1,
        MOMENTARY_OUT2,
        MOMENTARY_OUT3,
        MOMENTARY_OUT4,
        NUM_OUTPUTS
    };
    enum LightIds {
        RUN_LED,
        MOMENTARY_LED,
        NUM_LIGHTS
    };

    SchmittTrigger runTrigger;

    const float lightLambda = 0.075;
    float resetLight = 0.0;
    float output = 0.0;
    float volts=0;
    bool running = false;
    Triggers() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;


    json_t *toJson() override
    {
        json_t *rootJ = json_object();
        json_t *button_statesJ = json_array();
        json_t *button_stateJ = json_integer((int)running);
        json_array_append_new(button_statesJ, button_stateJ);		
        json_object_set_new(rootJ, "run", button_statesJ);    
        return rootJ;
    }

    void fromJson(json_t *rootJ) override
    {
        json_t *button_statesJ = json_object_get(rootJ, "run");
        if (button_statesJ)
        {			
                json_t *button_stateJ = json_array_get(button_statesJ,0);
                if (button_stateJ)
                    running = !!json_integer_value(button_stateJ);			
        }  
    }  
    
};

void Triggers::step() {

    output = 0.0;
    volts = 0.0;
    //RUN BUTTON STATUS
    if (runTrigger.process(params[RUN_SWITCH].value)){
        running = !running;
    }
    lights[RUN_LED].value = running ? 1.0 : 0.0;
     if (running){
        volts = clampf(params[VOLTAGE_PARAM].value, 1.0, 10.0);
	}
    outputs[RUN_OUTPUT1].value =  volts;
    outputs[RUN_OUTPUT2].value = volts;
    outputs[RUN_OUTPUT3].value = volts;
    outputs[RUN_OUTPUT4].value = volts;
    //MOMENTARY BUTTON STATUS
    if (params[MOMENTARY_SWITCH].value > 0.0){
          resetLight = 1.0;
          output = clampf(params[VOLTAGE_PARAM].value, 1.0, 10.0);
	}
    resetLight -= resetLight / lightLambda / engineGetSampleRate();
    lights[MOMENTARY_LED].value = resetLight;
    outputs[MOMENTARY_OUT1].value = output;
    outputs[MOMENTARY_OUT2].value = output;
    outputs[MOMENTARY_OUT3].value = output;
    outputs[MOMENTARY_OUT4].value = output;
}

TriggersWidget::TriggersWidget() {
    Triggers *module = new Triggers();
    setModule(module);
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Triggers.svg")));
        addChild(panel);
    }
	//SCREWS
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<as_HexScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<as_HexScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    //SWITCHES
    static const float led_offset = 3.3;
    static const float led_center = 15;
    addParam(createParam<BigLEDBezel>(Vec(led_center, 120), module, Triggers::RUN_SWITCH, 0.0, 1.0, 0.0));
    addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 120+led_offset), module, Triggers::RUN_LED));
    addParam(createParam<BigLEDBezel>(Vec(led_center, 232), module, Triggers::MOMENTARY_SWITCH, 0.0, 1.0, 0.0));
    addChild(createLight<GiantLight<RedLight>>(Vec(led_center+led_offset, 232+led_offset), module, Triggers::MOMENTARY_LED));
	//PARAMS
	addParam(createParam<as_KnobBlack>(Vec(26, 60), module, Triggers::VOLTAGE_PARAM, 1.0, 10.0, 5.0));

    //PORTS
    addOutput(createOutput<as_PJ301MPort>(Vec(10, 180), module, Triggers::RUN_OUTPUT1));
    addOutput(createOutput<as_PJ301MPort>(Vec(55, 180), module, Triggers::RUN_OUTPUT2));
    addOutput(createOutput<as_PJ301MPort>(Vec(10, 210), module, Triggers::RUN_OUTPUT3));
    addOutput(createOutput<as_PJ301MPort>(Vec(55, 210), module, Triggers::RUN_OUTPUT4));
    //PORTS
    addOutput(createOutput<as_PJ301MPort>(Vec(10, 290), module, Triggers::MOMENTARY_OUT1));
    addOutput(createOutput<as_PJ301MPort>(Vec(55, 290), module, Triggers::MOMENTARY_OUT2));
    addOutput(createOutput<as_PJ301MPort>(Vec(10, 320), module, Triggers::MOMENTARY_OUT3));
    addOutput(createOutput<as_PJ301MPort>(Vec(55, 320), module, Triggers::MOMENTARY_OUT4));

}
