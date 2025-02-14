using System.Collections.Generic;
using UnityEngine;

public class IncisionManager : MonoBehaviour
{
    private Mesh mesh;
    private Vector3[] vertices;
    private int[] triangles;
    private List<int> affectedTriangles = new List<int>();
    
    public float cutThreshold = 0.01f; // Sensitivity of the incision
    private bool isCutting = false;
    
    void Start()
    {
        mesh = GetComponent<MeshFilter>().mesh;
        vertices = mesh.vertices;
        triangles = mesh.triangles;
    }

    void Update()
    {
        if (isCutting)
        {
            ModifyMesh();
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("scalpel"))
        {
            isCutting = true;
            Vector3 contactPoint = other.transform.position;
            FindAffectedTriangles(contactPoint);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("scalpel"))
        {
            isCutting = false;
        }
    }

    void FindAffectedTriangles(Vector3 cutPoint)
    {
        affectedTriangles.Clear();

        for (int i = 0; i < triangles.Length; i += 3)
        {
            Vector3 v1 = transform.TransformPoint(vertices[triangles[i]]);
            Vector3 v2 = transform.TransformPoint(vertices[triangles[i + 1]]);
            Vector3 v3 = transform.TransformPoint(vertices[triangles[i + 2]]);

            if (IsTriangleNearCut(v1, v2, v3, cutPoint))
            {
                affectedTriangles.Add(i);
            }
        }
    }

    bool IsTriangleNearCut(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 cutPoint)
    {
        return (Vector3.Distance(v1, cutPoint) < cutThreshold ||
                Vector3.Distance(v2, cutPoint) < cutThreshold ||
                Vector3.Distance(v3, cutPoint) < cutThreshold);
    }

    void ModifyMesh()
    {
        List<Vector3> newVertices = new List<Vector3>(vertices);
        List<int> newTriangles = new List<int>(triangles);

        foreach (int triIndex in affectedTriangles)
        {
            int i1 = triangles[triIndex];
            int i2 = triangles[triIndex + 1];
            int i3 = triangles[triIndex + 2];

            // Slightly move affected vertices to simulate incision
            newVertices[i1] += Vector3.up * 0.005f;
            newVertices[i2] += Vector3.up * 0.005f;
            newVertices[i3] += Vector3.up * 0.005f;
        }

        mesh.vertices = newVertices.ToArray();
        mesh.RecalculateNormals();
        mesh.RecalculateBounds();
    }
}
