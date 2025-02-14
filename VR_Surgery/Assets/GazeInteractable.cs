using UnityEngine;

public class GazeInteractable : MonoBehaviour
{
    private Renderer objectRenderer;
    private Color originalColor;
    private bool isInFieldOfView = false; // Track if object is in FOV

    void Start()
    {
        objectRenderer = GetComponent<Renderer>();
        originalColor = objectRenderer.material.color;
    }

    // Called when the object is gazed at
    public void OnGazeEnter()
    {
        objectRenderer.material.color = Color.red;
        Debug.Log(gameObject.name + " is being gazed at! Color changed to RED.");
    }

    // Called when the gaze leaves the object
    public void OnGazeExit()
    {
        objectRenderer.material.color = originalColor;
        Debug.Log(gameObject.name + " gaze removed! Color reverted.");
    }

    // Method to update FOV status
    public void SetInFieldOfView(bool status)
    {
        if (isInFieldOfView != status)  // Only print when status changes
        {
            isInFieldOfView = status;
            if (isInFieldOfView)
                Debug.Log(gameObject.name + " is in field of view.");
            else
                Debug.Log(gameObject.name + " is out of field of view.");
        }
    }
}
