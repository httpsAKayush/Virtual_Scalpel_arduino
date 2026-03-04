using System.Collections;
using System.Collections.Generic;
using NUnit.Framework;
using UnityEngine;

public class GameManager : MonoBehaviour
{
   
    public Transform scalpel;
    public static GameManager instance;

    public GameManager()
    {
        instance= this;
    }

    public void MoveBox(float x)
    {
        var pos = scalpel.position;
        pos.x = x;
        scalpel.position = pos;
    }

}
