//
//  ViewController.swift
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        guard let fileString = Bundle.main.path(forResource: "test", ofType: "mp4") else { return  }
        let isExit = FileManager.default.fileExists(atPath: fileString)
        print(isExit)
        print(fileString)
        
        let pcmDirPath = NSHomeDirectory() + "/Documents/output/pcm"
        if !FileManager.default.fileExists(atPath: pcmDirPath) {
            do {
                try FileManager.default.createDirectory(atPath: pcmDirPath, withIntermediateDirectories: true, attributes: nil)
            } catch {}
        }
        FFmpegInvoke.decodeAudio(withInputFilePath: fileString, outputFilePath: pcmDirPath + "/test.pcm")
        
        let yuvDirPath = NSHomeDirectory() + "/Documents/output/yuv"
        if !FileManager.default.fileExists(atPath: yuvDirPath) {
            do {
                try FileManager.default.createDirectory(atPath: yuvDirPath, withIntermediateDirectories: true, attributes: nil)
            } catch {}
        }
        FFmpegInvoke.decodeVideo(withInputFilePath: fileString, outputFilePath: yuvDirPath + "/test.yuv")
    }


}

