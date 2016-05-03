//
//  SonicView.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//
//////////////***************************////////////////////////////////////////

import UIKit
import Charts

class SonicView: UIView {

    @IBOutlet var containerView: UIView!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var uvIndex: UILabel!
    @IBOutlet weak var lineChartView: LineChartView!
    @IBOutlet weak var resetButton: UIButton!
    @IBOutlet weak var resetBtnImage: UIImageView!
    @IBOutlet weak var putOnSunscreenLabel: UILabel!
    
    var myController : ConnectedViewController!
    
    @IBAction func resetButton(sender: UIButton) {
        
        
        for _ in 0 ..< 10 {
            delay(1.0){
                self.myController.connectedBean!.sendSerialString("reset")
            }
        }
        
        
        
        resetButton.setTitleColor( UIColor.grayColor(), forState: UIControlState.Normal)
        resetBtnImage.backgroundColor = UIColor.grayColor()
        
        resetButton.enabled  = false;
        
    }
    
    @IBAction func disconnectBeanButton(sender: UIButton) {
        
        myController.connectedBean?.beanManager.disconnectBean(myController.connectedBean, error: nil)
        
    }
    // MARK: Lifecycle
    
    func delay(delay:Double, closure:()->()) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW,Int64(delay * Double(NSEC_PER_SEC))),dispatch_get_main_queue(), closure)
    }

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        let bundle = NSBundle(forClass: self.dynamicType)
        self.addSubview(bundle.loadNibNamed("SonicView", owner: self, options: nil)[0] as! UIView)
    }

}
