using UnityEngine;
using System.Collections;

public class GazeSwitcher : MonoBehaviour
{
    public GameObject tool1; // First tool
    public GameObject tool2; // Second tool

    private float gazeTime = 0f; 
    public float requiredGazeTime = 5f; // Time required to switch tools
    private bool isGazing = false;

    void Start()
    {
        tool1.SetActive(true);  // Show tool1 initially
        tool2.SetActive(false); // Hide tool2
    }

    void Update()
    {
        if (isGazing)
        {
            gazeTime += Time.deltaTime; 

            if (gazeTime >= requiredGazeTime)
            {
                SwitchTools();
                gazeTime = 0f; // Reset gaze time
            }
        }
        else
        {
            gazeTime = 0f; // Reset if gaze is lost
        }
    }

    private void SwitchTools()
    {
        tool1.SetActive(!tool1.activeSelf); // Toggle visibility
        tool2.SetActive(!tool2.activeSelf);
    }

    // Called when the player starts gazing at the rectangle
    public void StartGaze()
    {
        isGazing = true;
    }

    // Called when the player stops gazing at the rectangle
    public void StopGaze()
    {
        isGazing = false;
    }
}
