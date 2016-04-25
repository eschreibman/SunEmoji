//
//  ConnectedViewController.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit

class ConnectedViewController: UIViewController, PTDBeanDelegate {

    var connectedBean: PTDBean?

    @IBOutlet weak var scrollView: UIScrollView!
    @IBOutlet weak var sonicView: SonicView!

    // MARK: Lifecycle

    override func viewDidLoad() {
        super.viewDidLoad()

        // Update the name label.
        sonicView.nameLabel.text = connectedBean?.name
        
        sonicView.myController = self;
    }

    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        connectedBean?.readTemperature()
    }

    // MARK: Actions

    func updateAcc(){
        connectedBean?.readTemperature()
        connectedBean?.readAccelerationAxes()
    }

    // MARK: PTDBeanDelegate
    
    func bean(bean: PTDBean!, serialDataReceived data:NSData) {
        let theString:String = NSString(data: data, encoding: NSASCIIStringEncoding)! as String
        
        if(theString.containsString("time:")){
            let stringParts = theString.componentsSeparatedByString(":")
            sonicView.distance.text = stringParts[1];
        }
        
        print(theString)
    }

    // MARK: Helper

    func updatesonicView() {


    }

    func updateBean() {
        connectedBean?.setLedColor(sonicView.containerView.backgroundColor)
    }

}