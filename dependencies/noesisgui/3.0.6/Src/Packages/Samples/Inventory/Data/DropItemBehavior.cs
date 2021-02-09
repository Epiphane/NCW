﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Interactivity;

namespace Inventory
{
    class DropItemBehavior : Behavior<FrameworkElement>
    {
        public bool IsDragOver
        {
            get { return (bool)GetValue(IsDragOverProperty); }
            set { SetValue(IsDragOverProperty, value); }
        }

        public static readonly DependencyProperty IsDragOverProperty = DependencyProperty.Register(
            "IsDragOver", typeof(bool), typeof(DropItemBehavior), new PropertyMetadata(false));

        public ICommand DropCommand
        {
            get { return (ICommand)GetValue(DropCommandProperty); }
            set { SetValue(DropCommandProperty, value); }
        }

        public static readonly DependencyProperty DropCommandProperty = DependencyProperty.Register(
            "DropCommand", typeof(ICommand), typeof(DropItemBehavior), new PropertyMetadata(null));

        protected override void OnAttached()
        {
            base.OnAttached();

            this.AssociatedObject.AllowDrop = true;
            this.AssociatedObject.PreviewDragEnter += OnDragEnter;
            this.AssociatedObject.PreviewDragLeave += OnDragLeave;
            this.AssociatedObject.PreviewDrop += OnDrop;
        }

        protected override void OnDetaching()
        {
            this.AssociatedObject.AllowDrop = false;
            this.AssociatedObject.PreviewDragEnter -= OnDragEnter;
            this.AssociatedObject.PreviewDragLeave -= OnDragLeave;
            this.AssociatedObject.PreviewDrop -= OnDrop;

            base.OnDetaching();
        }

        private void OnDragEnter(object sender, DragEventArgs e)
        {
            IsDragOver = true;
            e.Handled = true;
        }

        private void OnDragLeave(object sender, DragEventArgs e)
        {
            IsDragOver = false;
            e.Handled = true;
        }

        private void OnDrop(object sender, DragEventArgs e)
        {
            IsDragOver = false;

            object item = this.AssociatedObject.DataContext;
            if (item != null && DropCommand != null && DropCommand.CanExecute(item))
            {
                DropCommand.Execute(item);
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }

            e.Handled = true;
        }
    }
}
