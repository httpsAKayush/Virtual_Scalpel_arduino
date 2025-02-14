using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
public class GyroControl: MonoBehaviour
{
private bool gyroEnabled;
private Gyroscope gyro;
private GameObject CameraContainer;
private Quaternion rot;

private void Start(){
    CameraContainer=new GameObject("Camera Container");
    CameraContainer.transform.position=transform.position; 
    transform.SetParent(CameraContainer.transform);
    gyroEnabled=EnableGyro();
}
private bool EnableGyro(){
 if(SystemInfo.supportsGyroscope){
 gyro=Input.gyro;
 gyro.enabled=true;
 CameraContainer.transform.rotation=Quaternion.Euler(90f,90f,0f);
 rot=new Quaternion(0,0,1,0);
return true;
 }
 return false;
}
private void Update(){
if(gyroEnabled){
    transform.localRotation=gyro.attitude*rot;
}
} 
}