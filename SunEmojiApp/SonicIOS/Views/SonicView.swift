//
//  SonicView.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit

class SonicView: UIView {

    @IBOutlet var containerView: UIView!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var distance : UILabel!
    
    var myController : ConnectedViewController!

    @IBAction func sendChar(sender: UIButton) {
        
        let key_touched = sender.currentTitle
        print (key_touched! + " touched!")
        var morse_code : String = ".-" //Morse code for A
        myController.connectedBean!.sendSerialString("morse:" + morse_code)
    }
    
    @IBAction func colorChoice(sender: UISegmentedControl) {
        
        var color_string : String
        
        switch(sender.selectedSegmentIndex){
        case 0:
            color_string = "led:255,0,0"
        case 1:
            color_string = "led:0,255,0"
        case 2:
            color_string = "led:0,0,255"
        default:
            color_string = "";
        }
        myController.connectedBean!.sendSerialString(color_string)
        
    }
    // MARK: Lifecycle

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        let bundle = NSBundle(forClass: self.dynamicType)
        self.addSubview(bundle.loadNibNamed("SonicView", owner: self, options: nil)[0] as! UIView)
    }

}
