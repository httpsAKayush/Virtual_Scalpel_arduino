using UnityEngine;
using System.Collections.Generic;

public class IncisionEffect : MonoBehaviour
{
    public float cutRadius = 0.01f;

    void OnTriggerEnter(Collider other)
    {
        Debug.Log($"[Incision] Tool collided with: {other.name}");

        if (other.TryGetComponent(out MeshFilter mf))
        {
            Debug.Log("[Incision] MeshFilter found on collided object.");

            Mesh mesh = mf.mesh;
            if (mesh == null)
            {
                Debug.LogWarning("[Incision] Mesh is null!");
                return;
            }

            Vector3[] vertices = mesh.vertices;
            int[] triangles = mesh.triangles;

            // 🔴 NEW PART: Initialize vertex colors if missing
            if (mesh.colors == null || mesh.colors.Length != mesh.vertexCount)
            {
                Color[] cols = new Color[mesh.vertexCount];
                for (int i = 0; i < cols.Length; i++)
                    cols[i] = Color.black; // no cut initially
                mesh.colors = cols;
            }

            Color[] colors = mesh.colors; // reference after initialization
            List<int> updatedTriangles = new List<int>();

            Vector3 localToolPos = mf.transform.InverseTransformPoint(transform.position);
            Debug.Log($"[Incision] Tool local position in mesh space: {localToolPos}");

            int originalTriangleCount = triangles.Length / 3;
            int removedTriangleCount = 0;

            for (int i = 0; i < triangles.Length; i += 3)
            {
                Vector3 v0 = vertices[triangles[i]];
                Vector3 v1 = vertices[triangles[i + 1]];
                Vector3 v2 = vertices[triangles[i + 2]];
                Vector3 center = (v0 + v1 + v2) / 3f;

                if (Vector3.Distance(center, localToolPos) > cutRadius)
                {
                    // Keep triangle
                    updatedTriangles.Add(triangles[i]);
                    updatedTriangles.Add(triangles[i + 1]);
                    updatedTriangles.Add(triangles[i + 2]);
                }
                else
                {
                    // 🔴 Mark cut edge vertices (for shader)
                    colors[triangles[i]]     = Color.red;
                    colors[triangles[i + 1]] = Color.red;
                    colors[triangles[i + 2]] = Color.red;

                    removedTriangleCount++;
                }
            }

            // Apply updated mesh data
            mesh.triangles = updatedTriangles.ToArray();
            mesh.colors = colors;               // 🔴 IMPORTANT
            mesh.RecalculateNormals();

            Debug.Log($"[Incision] Total triangles before: {originalTriangleCount}, after cut: {updatedTriangles.Count / 3}");
            Debug.Log($"[Incision] Triangles removed (cut): {removedTriangleCount}");
        }
        else
        {
            Debug.LogWarning("[Incision] No MeshFilter found on collided object.");
        }
    }

    void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        Gizmos.DrawWireSphere(transform.position, cutRadius);
    }
}