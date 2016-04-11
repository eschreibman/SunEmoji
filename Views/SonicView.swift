//
//  SonicView.swift
//  Cool Beans
//
//  Created by Kyle on 11/14/14.
//  Copyright (c) 2014 Kyle Weiner. All rights reserved.
//

import UIKit
import Charts

class SonicView: UIView {

    @IBOutlet var containerView: UIView!
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var uvIndex: UILabel!
    @IBOutlet weak var lineChartView: LineChartView!
    
    var myController : ConnectedViewController!
    
    
    // MARK: Lifecycle

    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        let bundle = NSBundle(forClass: self.dynamicType)
        self.addSubview(bundle.loadNibNamed("SonicView", owner: self, options: nil)[0] as! UIView)
    }

}
