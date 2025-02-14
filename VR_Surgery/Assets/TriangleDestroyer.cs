using UnityEngine;

public class TriangleDestroyer : MonoBehaviour
{
    private void OnTriggerEnter(Collider other)
    {
        // Check if the colliding object has the tag "scalpel"
        if (other.CompareTag("scalpel"))
        {
            Destroy(gameObject); // Destroy the triangle
        }
    }
}
