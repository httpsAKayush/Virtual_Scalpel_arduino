using UnityEngine;

public class ScalpelController : MonoBehaviour
{
    public float speed = 0.05f;

    void Update()
    {
        HandleMovement();
    }

    void HandleMovement()
    {
        float moveX = Input.GetAxis("Horizontal") * speed;
        float moveY = Input.GetAxis("Vertical") * speed;
        float moveZ = Input.GetAxis("Depth") * speed;

        transform.Translate(new Vector3(moveX, moveY, moveZ));
    }
}
