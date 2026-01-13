import express, { Router, Request, Response } from 'express';
import { query } from '../services/database.js';

const router: Router = express.Router();

// Get compliance reports
router.get('/reports', async (req: Request, res: Response) => {
  try {
    const { framework, limit = 10 } = req.query;

    let queryText = 'SELECT * FROM compliance_reports WHERE 1=1';
    const params: any[] = [];
    let paramIndex = 1;

    if (framework) {
      queryText += ` AND framework = $${paramIndex}`;
      params.push(framework);
      paramIndex++;
    }

    queryText += ` ORDER BY generated_at DESC LIMIT $${paramIndex}`;
    params.push(Number(limit));

    const result = await query(queryText, params);

    res.json({
      reports: result.rows,
      total: result.rowCount,
    });
  } catch (error) {
    console.error('Get compliance reports error:', error);
    res.status(500).json({ error: 'Failed to fetch reports' });
  }
});

// Generate compliance report
router.post('/reports/generate', async (req: Request, res: Response) => {
  try {
    const { userId, framework } = req.body;

    // Simulate compliance assessment
    const mockFindings = generateMockComplianceFindings(framework);
    const score = calculateComplianceScore(mockFindings);

    const result = await query(
      `INSERT INTO compliance_reports (user_id, framework, score, findings)
       VALUES ($1, $2, $3, $4)
       RETURNING *`,
      [userId, framework, score, JSON.stringify(mockFindings)]
    );

    res.status(201).json(result.rows[0]);
  } catch (error) {
    console.error('Generate report error:', error);
    res.status(500).json({ error: 'Failed to generate report' });
  }
});

// Get compliance dashboard
router.get('/dashboard', async (req: Request, res: Response) => {
  try {
    const frameworks = ['SOC 2', 'ISO 27001', 'NIST CSF', 'GDPR', 'HIPAA'];
    
    const dashboardData = {
      overallScore: 87,
      frameworks: frameworks.map(framework => ({
        name: framework,
        score: Math.floor(Math.random() * 30) + 70,
        status: Math.random() > 0.3 ? 'compliant' : 'needs-attention',
        lastAssessment: new Date(Date.now() - Math.random() * 30 * 24 * 60 * 60 * 1000).toISOString(),
      })),
      recentFindings: [
        {
          id: 1,
          framework: 'SOC 2',
          severity: 'high',
          control: 'CC6.1',
          description: 'Access control policies need review',
          status: 'open',
        },
        {
          id: 2,
          framework: 'NIST CSF',
          severity: 'medium',
          control: 'PR.AC-1',
          description: 'Multi-factor authentication not enabled for all users',
          status: 'in-progress',
        },
      ],
      controls: {
        total: 150,
        compliant: 131,
        partiallyCompliant: 12,
        nonCompliant: 7,
      },
    };

    res.json(dashboardData);
  } catch (error) {
    console.error('Get compliance dashboard error:', error);
    res.status(500).json({ error: 'Failed to fetch dashboard' });
  }
});

// Helper functions
function generateMockComplianceFindings(framework: string) {
  return {
    controls: [
      {
        id: 'AC-1',
        name: 'Access Control Policy',
        status: 'compliant',
        evidence: ['Policy document reviewed', 'Annual review completed'],
      },
      {
        id: 'AC-2',
        name: 'Account Management',
        status: 'partially-compliant',
        evidence: ['User provisioning automated'],
        gaps: ['Manual review process needs improvement'],
      },
      {
        id: 'SI-2',
        name: 'Flaw Remediation',
        status: 'non-compliant',
        gaps: ['Patch management process not documented'],
      },
    ],
    summary: {
      totalControls: 50,
      compliant: 35,
      partiallyCompliant: 10,
      nonCompliant: 5,
    },
  };
}

function calculateComplianceScore(findings: any): number {
  const { compliant, partiallyCompliant, totalControls } = findings.summary;
  return Math.round(((compliant + partiallyCompliant * 0.5) / totalControls) * 100);
}

export default router;
