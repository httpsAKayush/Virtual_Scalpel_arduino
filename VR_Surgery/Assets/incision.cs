    using System.Collections.Generic;
using UnityEngine;

public class IncisionEffect : MonoBehaviour
{
    private Mesh mesh;
    private ComputeBuffer collisionBuffer;
    private int[] collisionData;
    private Material incisionMaterial;
    private List<int> affectedTriangles = new List<int>();

    void Start()
    {
        mesh = GetComponent<MeshFilter>().mesh;
        incisionMaterial = GetComponent<MeshRenderer>().material;

        // Initialize buffer (0 = no collision)
        collisionData = new int[mesh.triangles.Length / 3]; // One entry per triangle
        collisionBuffer = new ComputeBuffer(collisionData.Length, sizeof(int));
        collisionBuffer.SetData(collisionData);
        incisionMaterial.SetBuffer("_CollisionBuffer", collisionBuffer);
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("scalpel"))
        {
            Vector3 hitPoint = other.ClosestPoint(transform.position);
            MarkTrianglesForRemoval(hitPoint);
        }
    }

    void MarkTrianglesForRemoval(Vector3 hitPosition)
    {
        Vector3[] vertices = mesh.vertices;
        int[] triangles = mesh.triangles;

        for (int i = 0; i < triangles.Length; i += 3)
        {
            Vector3 v1 = transform.TransformPoint(vertices[triangles[i]]);
            Vector3 v2 = transform.TransformPoint(vertices[triangles[i + 1]]);
            Vector3 v3 = transform.TransformPoint(vertices[triangles[i + 2]]);

            if (IsTriangleColliding(v1, v2, v3, hitPosition))
            {
                affectedTriangles.Add(i / 3); // Store affected triangle ID
            }
        }

        // Update collision buffer
        foreach (int index in affectedTriangles)
        {
            collisionData[index] = 1; // Mark for removal
        }
        collisionBuffer.SetData(collisionData);
    }

    bool IsTriangleColliding(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 point)
    {
        float threshold = 0.1f; // Adjust as needed
        return (Vector3.Distance(point, v1) < threshold ||
                Vector3.Distance(point, v2) < threshold ||
                Vector3.Distance(point, v3) < threshold);
    }

    void OnDestroy()
    {
        if (collisionBuffer != null) collisionBuffer.Release();
    }
}