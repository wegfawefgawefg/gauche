import importlib.util
import tempfile
import unittest
from pathlib import Path
from aiohttp.test_utils import TestClient, TestServer

spec=importlib.util.spec_from_file_location('reports',Path(__file__).resolve().parents[1]/'services/browser-reports/server.py')
reports=importlib.util.module_from_spec(spec);spec.loader.exec_module(reports)

class ReportsTest(unittest.IsolatedAsyncioTestCase):
    async def test_validation_and_bounds(self):
        with tempfile.TemporaryDirectory() as directory:
            async with TestClient(TestServer(reports.create_app(directory))) as client:
                data={'version':1,'build':'abcd123','kind':'join-failed','message':'','browser':'test',
                      'state':{'seed':'1','floor':0,'tick':20},'timing':{},'events':[],'network':['ms=1 event=session_begin owner=0 value=2']}
                self.assertEqual((await client.post('/browser-reports',json=data)).status,403)
                headers={'Origin':'https://teeming.pages.dev','X-Real-IP':'test-ip'}
                self.assertEqual((await client.post('/browser-reports',json=data,headers=headers)).status,204)
                self.assertEqual((await client.get('/browser-reports',headers=headers)).status,405)
                invalid={**data,'state':{'room':'SECRET'}}
                self.assertEqual((await client.post('/browser-reports',json=invalid,headers=headers)).status,400)
                oversized={**data,'message':'x'*40000}
                self.assertEqual((await client.post('/browser-reports',json=oversized,headers=headers)).status,413)
                for _ in range(25):response=await client.post('/browser-reports',json=data,headers=headers)
                self.assertEqual(response.status,429)
                stored=(Path(directory)/'reports.jsonl').read_text()
                self.assertNotIn('test-ip',stored);self.assertNotIn('SECRET',stored)
                self.assertIn('join-failed',stored)

if __name__=='__main__':unittest.main()
