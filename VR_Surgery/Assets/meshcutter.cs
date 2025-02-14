using UnityEngine;
using EzySlice;

public class MeshCutter : MonoBehaviour
{
    public Material cutMaterial;  // Expose cut material to the Inspector
    public float moveSpeed = 5f;  // Expose move speed to the Inspector

    void Update()
    {
        // Move Cutter with Arrow Keys or WASD
        float moveX = Input.GetAxis("Horizontal") * moveSpeed * Time.deltaTime;
        float moveZ = Input.GetAxis("Vertical") * moveSpeed * Time.deltaTime;
        transform.position += new Vector3(moveX, 0, moveZ);

        // Rotate Cutter with Q & E keys
        if (Input.GetKey(KeyCode.Q)) transform.Rotate(Vector3.up, 1f);
        if (Input.GetKey(KeyCode.E)) transform.Rotate(Vector3.up, -1f);

        // Press Space to Cut
        if (Input.GetKeyDown(KeyCode.Space))
        {
            Debug.Log("Space pressed, starting cut...");
            CutMesh();
        }
    }

    void CutMesh()
    {
        GameObject cube = GameObject.Find("Cube");

        if (cube != null)
        {
            Debug.Log("Cube found, starting the slicing process...");

            // Create a Plane for slicing, using EzySlice.Plane
            EzySlice.Plane cuttingPlane = new EzySlice.Plane(transform.up, transform.position);
            Debug.Log($"Cutting plane created at position: {transform.position}, with normal: {transform.up}");

            // Slice the cube using EzySlice at the cutter's position and orientation
            SlicedHull slicedHull = cube.Slice(cuttingPlane, cutMaterial);

            if (slicedHull != null)
            {
                Debug.Log("Slicing successful, creating hulls...");

                // Create upper and lower parts of the sliced cube
                GameObject upperHalf = slicedHull.CreateUpperHull(cube, cutMaterial);
                GameObject lowerHalf = slicedHull.CreateLowerHull(cube, cutMaterial);

                Debug.Log("Upper and lower halves created");

                // Add Mesh Colliders (make sure they are convex for physics interaction)
                upperHalf.AddComponent<MeshCollider>().convex = true;
                lowerHalf.AddComponent<MeshCollider>().convex = true;

                // Add Rigidbody components so the parts fall (if gravity is enabled)
                upperHalf.AddComponent<Rigidbody>();
                lowerHalf.AddComponent<Rigidbody>();

                Debug.Log("Added Rigidbody and MeshCollider to sliced parts");

                // Destroy the original cube after it’s sliced
                Destroy(cube);
                Debug.Log("Original cube destroyed");
            }
            else
            {
                Debug.LogError("SlicedHull is null. Make sure the cutter is correctly aligned with the cube.");
            }
        }
        else
        {
            Debug.LogError("Cube not found");
        }
    }
}