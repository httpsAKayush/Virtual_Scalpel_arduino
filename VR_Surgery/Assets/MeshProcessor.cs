using UnityEngine;
using System.Collections.Generic;

public class MeshProcessor : MonoBehaviour
{
    void Start()
    {
        // Get the MeshFilter component
        MeshFilter mf = GetComponent<MeshFilter>();
        if (mf == null) return;
        
        Mesh originalMesh = mf.mesh;
        Vector3[] vertices = originalMesh.vertices;
        int[] triangles = originalMesh.triangles;

        for (int i = 0; i < triangles.Length; i += 3)
        {
            // Create a new GameObject for each triangle
            GameObject triangleObj = new GameObject("Triangle_" + (i / 3));
            MeshFilter newMF = triangleObj.AddComponent<MeshFilter>();
            MeshRenderer newMR = triangleObj.AddComponent<MeshRenderer>();
            MeshCollider newMC = triangleObj.AddComponent<MeshCollider>(); // Add collider
            newMC.convex = true; // Ensure it can detect collisions
            newMC.isTrigger = true; // Set it to trigger mode

            // Assign the same material
            newMR.material = GetComponent<MeshRenderer>().material;

            // Create a new mesh for this triangle
            Mesh newMesh = new Mesh();
            Vector3[] newVertices = new Vector3[3];
            int[] newTriangles = new int[3];

            for (int j = 0; j < 3; j++)
            {
                newVertices[j] = vertices[triangles[i + j]];
                newTriangles[j] = j;
            }

            

            newMesh.vertices = newVertices;
            newMesh.triangles = newTriangles;
            newMesh.RecalculateNormals();

            newMF.mesh = newMesh;
            newMC.sharedMesh = newMesh; // Assign collider the same mesh

            // Set parent for better organization
            triangleObj.transform.SetParent(transform);
            triangleObj.transform.localPosition = Vector3.zero;

            // Add the script for detecting collision
            triangleObj.AddComponent<TriangleDestroyer>();
        }

        

        // Disable the original model
        gameObject.SetActive(false);
    }
}


