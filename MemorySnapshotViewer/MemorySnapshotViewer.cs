/**
 * @author 李锦俊
 * @email mybios@qq.com
 */
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Xml.Linq;
using System.Collections.Generic;

namespace MemorySnapshotViewer
{
    /// <summary>
    /// Summary description for MemorySnapshotViewer.
    /// </summary>
    public class MemorySnapshotViewer : System.Windows.Forms.Form
    {
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private ColumnHeader allocRawSize;
        private ColumnHeader allocBytes;
        private ColumnHeader allocCount;
        private ColumnHeader sumAllocBytes;
        private ColumnHeader sumAllocCount;
        private ColumnHeader sumDeallocBytes;
        private ColumnHeader sumDeallocCount;
        private ColumnHeader allocType;
        private ColumnHeader name;
        private ListView treeListView1;
        private System.ComponentModel.IContainer components;

        public MemorySnapshotViewer()
        {
            //
            // Required for Windows Form Designer support
            //
            InitializeComponent();

            //
            // TODO: Add any constructor code after InitializeComponent call
            //
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose( bool disposing )
        {
            if( disposing )
            {
                if(components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose( disposing );
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MemorySnapshotViewer));
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.treeListView1 = new System.Windows.Forms.ListView();
            this.name = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.allocRawSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.allocBytes = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.allocCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.sumAllocBytes = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.sumAllocCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.sumDeallocBytes = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.sumDeallocCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.allocType = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.SuspendLayout();
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "");
            this.imageList1.Images.SetKeyName(1, "");
            this.imageList1.Images.SetKeyName(2, "");
            this.imageList1.Images.SetKeyName(3, "");
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.button1.Location = new System.Drawing.Point(21, 637);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(291, 49);
            this.button1.TabIndex = 1;
            this.button1.Text = "��Snapshot";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.button2.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.button2.Location = new System.Drawing.Point(838, 637);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(333, 49);
            this.button2.TabIndex = 2;
            this.button2.Text = "Expand / Collapse All";
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // treeListView1
            // 
            this.treeListView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.treeListView1.CheckBoxes = true;
            this.treeListView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.name,
            this.allocRawSize,
            this.allocBytes,
            this.allocCount,
            this.sumAllocBytes,
            this.sumAllocCount,
            this.sumDeallocBytes,
            this.sumDeallocCount,
            this.allocType});
            this.treeListView1.FullRowSelect = true;
            this.treeListView1.GridLines = true;
            this.treeListView1.HideSelection = false;
            this.treeListView1.Location = new System.Drawing.Point(0, 0);
            this.treeListView1.Name = "treeListView1";
            this.treeListView1.Size = new System.Drawing.Size(1193, 631);
            this.treeListView1.TabIndex = 0;
            this.treeListView1.UseCompatibleStateImageBehavior = false;
            this.treeListView1.View = System.Windows.Forms.View.Details;
            this.treeListView1.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.treeListView1_ItemChecked);
            // 
            // name
            // 
            this.name.Text = "name";
            this.name.Width = 250;
            // 
            // allocRawSize
            // 
            this.allocRawSize.Text = "allocRawSize";
            this.allocRawSize.Width = 130;
            // 
            // allocBytes
            // 
            this.allocBytes.Text = "allocBytes";
            this.allocBytes.Width = 130;
            // 
            // allocCount
            // 
            this.allocCount.Text = "allocCount";
            this.allocCount.Width = 130;
            // 
            // sumAllocBytes
            // 
            this.sumAllocBytes.Text = "sumAllocBytes";
            this.sumAllocBytes.Width = 130;
            // 
            // sumAllocCount
            // 
            this.sumAllocCount.Text = "sumAllocCount";
            this.sumAllocCount.Width = 130;
            // 
            // sumDeallocBytes
            // 
            this.sumDeallocBytes.Text = "sumDeallocBytes";
            this.sumDeallocBytes.Width = 130;
            // 
            // sumDeallocCount
            // 
            this.sumDeallocCount.Text = "sumDeallocCount";
            this.sumDeallocCount.Width = 130;
            // 
            // allocType
            // 
            this.allocType.Text = "allocType";
            this.allocType.Width = 130;
            // 
            // MemorySnapshotViewer
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(13, 28);
            this.ClientSize = new System.Drawing.Size(1192, 695);
            this.Controls.Add(this.treeListView1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Name = "MemorySnapshotViewer";
            this.Text = "MemorySnapshotViewer";
            this.Load += new System.EventHandler(this.MemorySnapshotViewer_Load);
            this.ResumeLayout(false);

        }
        #endregion

        [STAThread]
        static void Main() 
        {
            Application.Run(new MemorySnapshotViewer());
        }

        // Implements the manual sorting of items by columns.
        class NodeComparer : IComparer<Node>
        {
            public SortOrder Order = SortOrder.Descending;
            public int SortColumn = 1;
            public int Compare(Node x, Node y)
            {
                int result = x.GetSubItem(SortColumn) - y.GetSubItem(SortColumn);
                return Order == SortOrder.Ascending ? result : -result;
            }
        }
        NodeComparer lvwColumnSorter;

        private void MemorySnapshotViewer_Load(object sender, System.EventArgs e)
        {
            lvwColumnSorter = new NodeComparer();
            treeListView1.ColumnClick += TreeListView1_ColumnClick;

        }

        private void TreeListView1_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            var col = treeListView1.Columns[lvwColumnSorter.SortColumn];
            col.Text = col.Text.Substring(0, col.Text.Length - (lvwColumnSorter.Order == SortOrder.Ascending ? "��" : "��").Length);

            // ����������ǲ������ڵ�������.
            if (e.Column == lvwColumnSorter.SortColumn)
            {
                // �������ô��е����򷽷�.
                if (lvwColumnSorter.Order == SortOrder.Ascending)
                {
                    lvwColumnSorter.Order = SortOrder.Descending;
                }
                else
                {
                    lvwColumnSorter.Order = SortOrder.Ascending;
                }
            }
            else
            {
                // ���������У�Ĭ��Ϊ��������
                lvwColumnSorter.SortColumn = e.Column;
                lvwColumnSorter.Order = SortOrder.Ascending;
            }

            // ���µ����򷽷���ListView����
            ResortNodes();
        }


        private void button1_Click(object sender, System.EventArgs e)
        {
            //�½�һ���ļ��Ի���
            OpenFileDialog pOpenFileDialog = new OpenFileDialog();
            //���öԻ������
            pOpenFileDialog.Title = "��Snapshot�ļ�";
            //���ô��ļ�����
            pOpenFileDialog.Filter = "Snapshot�ļ���*.xml��|*.xml";
            //����ļ��Ƿ����
            pOpenFileDialog.CheckFileExists = true;
            pOpenFileDialog.Multiselect = false;
            //�ļ��򿪺�ִ�����³���
            if (pOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                treeListView1.Items.Clear();
                LoadXmlTree(pOpenFileDialog.FileName);
            }
        }

        bool _allopen = false;
        private void button2_Click(object sender, System.EventArgs e)
        {
            _allopen = !_allopen;
            for(var index = 0; index < treeListView1.Items.Count; index ++)
            {
                treeListView1.Items[index].Checked = _allopen;
            }
            _root.setOpenAll(_allopen);
        }

    

        Node _root;

        public void LoadXmlTree(string xml)
        {
            try
            {
                XDocument xDoc = XDocument.Load(xml);
                treeListView1.LabelEdit = false;
                _root = CreateNode(null, xDoc.Root, 0);
                ResortNodes();
                this.Text = xml;
                _allopen = true;
            }
            catch(Exception e)
            {
                MessageBox.Show("��Snapshot�ļ�[" + xml + "]ʧ�ܣ�" + e.ToString());

            }
        }

        const int LUA_TBOOLEAN = 1;
        const int LUA_TLIGHTUSERDATA = 2;
        const int LUA_TNUMBER = 3;
        const int LUA_TSTRING = 4;
        const int LUA_TTABLE = 5;
        const int LUA_TFUNCTION = 6;
        const int LUA_TUSERDATA = 7;
        const int LUA_TTHREAD = 8;

        public class Node
        {
            public bool open = true;
            public int level;
            public Node parent;
            public string name;
            public int rawSize;
            public int allocBytes;
            public int allocCount;
            public int sumAllocBytes;
            public int sumAllocCount;
            public int sumDeallocBytes;
            public int sumDeallocCount;
            public int allocType;
            public int GetSubItem(int index)
            {
                switch(index)
                {
                     case 1:return rawSize;
                     case 2:return allocBytes;
                     case 3:return allocCount;
                     case 4:return sumAllocBytes;
                     case 5:return sumAllocCount;
                     case 6:return sumDeallocBytes;
                     case 7:return sumDeallocCount;
                     case 8:return allocType;
                }
                return 0;
            }

            public List<Node> children = new List<Node>();

            public int allocRawSize
            {
                get
                {
                    return allocBytes + rawSize;
                }
            }
            public string displayName
            {
                get
                {
                    return new String(' ', level * 4) + (children.Count > 0 ? (open ? "- " : "+ ") : "| ") + name;
                }
            }
            public string displayAllocType
            {
                get
                {
                    string typeString = allocType.ToString();
                    if ((allocType & (1 << LUA_TBOOLEAN)) != 0) typeString += "|boolean";
                    if ((allocType & (1 << LUA_TLIGHTUSERDATA)) != 0) typeString += "|lightuserdata";
                    if ((allocType & (1 << LUA_TNUMBER)) != 0) typeString += "|number";
                    if ((allocType & (1 << LUA_TSTRING)) != 0) typeString += "|string";
                    if ((allocType & (1 << LUA_TTABLE)) != 0) typeString += "|table";
                    if ((allocType & (1 << LUA_TFUNCTION)) != 0) typeString += "|function";
                    if ((allocType & (1 << LUA_TUSERDATA)) != 0) typeString += "|userdate";
                    if ((allocType & (1 << LUA_TTHREAD)) != 0) typeString += "|thread";
                    return typeString;
                }
            }
            public void setOpenAll(bool open)
            {
                this.open = open;
                foreach (var child in children)
                    child.setOpenAll(open);
            }
        }

        public void ResortNodes()
        {
            treeListView1.Columns[lvwColumnSorter.SortColumn].Text += lvwColumnSorter.Order == SortOrder.Ascending ? "��" : "��";

            treeListView1.BeginUpdate();
            isUpdating = true;
            treeListView1.Items.Clear();
            PopulateTree(_root, treeListView1.Items, 0);
            isUpdating = false;
            treeListView1.EndUpdate();
        }

        public Node CreateNode(Node parent, XElement element, int level)
        {
            var node = new Node();
            node.parent = parent;
            node.level = level;
            node.name = element.Attribute("name")?.Value;
            node.allocBytes = int.Parse(element.Attribute("allocBytes")?.Value);
            node.rawSize = int.Parse(element.Attribute("rawSize")?.Value);
            node.allocCount = int.Parse(element.Attribute("allocCount")?.Value);
            node.sumAllocBytes = int.Parse(element.Attribute("sumAllocBytes")?.Value);
            node.sumAllocCount = int.Parse(element.Attribute("sumAllocCount")?.Value);
            node.sumDeallocBytes = int.Parse(element.Attribute("sumDeallocBytes")?.Value);
            node.sumDeallocCount = int.Parse(element.Attribute("sumDeallocCount")?.Value);
            node.allocType = int.Parse(element.Attribute("allocType")?.Value);

            if (element.HasElements)
            {
                level++;
                foreach (XElement xnode in element.Nodes())
                {
                    node.children.Add(CreateNode(node, xnode, level));
                }
                level--;
            }
            return node;
        }
           
        public int PopulateTree(Node node, ListView.ListViewItemCollection items, int index)
        {
            ListViewItem item = new ListViewItem();
            item.Text = node.displayName;
            item.Checked = node.open;
            item.SubItems.Add(node.allocRawSize.ToString());
            item.SubItems.Add(node.allocBytes.ToString());
            item.SubItems.Add(node.allocCount.ToString());
            item.SubItems.Add(node.sumAllocBytes.ToString());
            item.SubItems.Add(node.sumAllocCount.ToString());
            item.SubItems.Add(node.sumDeallocBytes.ToString());
            item.SubItems.Add(node.sumDeallocCount.ToString());
            item.SubItems.Add(node.displayAllocType);

            item.Tag = node;
            items.Insert(index, item);
            index++;
            node.children.Sort(lvwColumnSorter);
            if (node.open)
            {
                foreach (var child in node.children)
                {
                    index = PopulateTree(child, items, index);
                }
            }
            return index;
        }

        bool removeChildren(Node parent, ListViewItem item)
        {
            Node node = (Node)item.Tag;

            bool isParent = false;
            for(Node p = node.parent; p != null; p = p.parent)
            {
                if (p == parent) isParent = true;
            }
            if(isParent)
            {
                treeListView1.Items.Remove(item);
                return true;
            }
            return false;
        }

        bool isUpdating;

        private void treeListView1_ItemChecked(object sender, ItemCheckedEventArgs e)
        {
            if (isUpdating)
                return;
            Node node = (Node)e.Item.Tag;
            node.open = e.Item.Checked;

            treeListView1.BeginUpdate();
            isUpdating = true;
            if (node.open)
            {
                int index = e.Item.Index + 1;
                foreach (var child in node.children)
                {
                    index = PopulateTree(child, treeListView1.Items, index);
                }
            }
            else
            {
                ListViewItem item = e.Item;
                while (item.Index + 1 < treeListView1.Items.Count)
                {
                    item = treeListView1.Items[item.Index + 1];
                    removeChildren(node, item);
                }
            }
            e.Item.Text = node.displayName;
            isUpdating = false;
            treeListView1.EndUpdate();
        }
    }
}
